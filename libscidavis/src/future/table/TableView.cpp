/***************************************************************************
    File                 : TableView.cpp
    Project              : SciDAVis
    Description          : View class for Table
    --------------------------------------------------------------------
    Copyright            : (C) 2007 Tilman Benkert (thzs*gmx.net)
                           (replace * with @ in the email addresses)

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/

#include "table/future_Table.h"
#include "table/TableView.h"
#include "table/TableModel.h"
#include "table/TableItemDelegate.h"
#include "table/tablecommands.h"
#include "table/TableDoubleHeaderView.h"

#include "core/column/Column.h"
#include "core/AbstractFilter.h"
#include "core/datatypes/SimpleCopyThroughFilter.h"
#include "core/datatypes/Double2StringFilter.h"
#include "core/datatypes/String2DoubleFilter.h"
#include "core/datatypes/DateTime2StringFilter.h"
#include "core/datatypes/String2DateTimeFilter.h"
#include "core/datatypes/Double2DateTimeFilter.h"

#include <QKeyEvent>
#include <QtDebug>
#include <QHeaderView>
#include <QRect>
#include <QPoint>
#include <QSize>
#include <QFontMetrics>
#include <QFont>
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QShortcut>
#include <QModelIndex>
#include <QGridLayout>
#include <QScrollArea>
#include <QMenu>
#include "ApplicationWindow.h"

#ifndef LEGACY_CODE_0_2_x
TableView::TableView(future::Table *table) : d_table(table)
#else
TableView::TableView(const QString &label, QWidget *parent, const QString name, Qt::WindowFlags f)
    : MyWidget(label, parent, name, f)
#endif
{
#ifndef LEGACY_CODE_0_2_x
    d_model = new TableModel(table);
    init();
#else
    d_model = NULL;
#endif
}

TableView::~TableView()
{
    delete d_model;
}

void TableView::setTable(future::Table *table)
{
    if (nullptr != table) {
        d_table = table;
        d_model = new TableModel(table);
        init();
        d_table->setView(this);
    }
}

void TableView::init()
{
    QWidget *d_main_widget = new QWidget();
    d_main_layout = new QHBoxLayout();
    d_main_widget->setLayout(d_main_layout);
    d_main_layout->setSpacing(0);
    d_main_layout->setContentsMargins(0, 0, 0, 0);

    d_view_widget = new TableViewWidget(this);
    auto &settings = ApplicationWindow::getSettings();
    settings.beginGroup("[Table]");
    int defaultRowHeight = settings.value("DefaultRowHeight", 20).toInt();
    settings.endGroup();
    d_view_widget->verticalHeader()->setDefaultSectionSize(defaultRowHeight);
    d_view_widget->setModel(d_model);
    connect(d_view_widget, SIGNAL(advanceCell()), this, SLOT(advanceCell()));
    d_main_layout->addWidget(d_view_widget);

    d_horizontal_header = new TableDoubleHeaderView();
#if QT_VERSION >= 0x050000
    d_horizontal_header->setSectionsClickable(true);
#else
    d_horizontal_header->setClickable(true);
#endif
    d_horizontal_header->setHighlightSections(true);
    d_view_widget->setHorizontalHeader(d_horizontal_header);

    d_hide_button = new QToolButton();
    d_hide_button->setArrowType(Qt::RightArrow);
    d_hide_button->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding));
    d_hide_button->setCheckable(false);
    d_main_layout->addWidget(d_hide_button);
    connect(d_hide_button, SIGNAL(pressed()), this, SLOT(toggleControlTabBar()));
    d_control_tabs = new QWidget();
    ui.setupUi(d_control_tabs);
    d_main_layout->addWidget(d_control_tabs);

    d_delegate = new TableItemDelegate(d_view_widget);
    d_view_widget->setItemDelegate(d_delegate);

    d_view_widget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    d_main_layout->setStretchFactor(d_view_widget, 1);

    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    d_view_widget->setFocusPolicy(Qt::StrongFocus);
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
#if QT_VERSION >= 0x040300
    d_view_widget->setCornerButtonEnabled(true);
#endif

    d_view_widget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    QHeaderView *v_header = d_view_widget->verticalHeader();
#if QT_VERSION >= 0x050000
    v_header->setSectionResizeMode(QHeaderView::Interactive);
    v_header->setSectionsMovable(false);
    d_horizontal_header->setSectionResizeMode(QHeaderView::Interactive);
    d_horizontal_header->setSectionsMovable(true);
#else
    v_header->setResizeMode(QHeaderView::Interactive);
    v_header->setMovable(false);
    d_horizontal_header->setResizeMode(QHeaderView::Interactive);
    d_horizontal_header->setMovable(true);
#endif
    connect(d_horizontal_header, SIGNAL(sectionMoved(int, int, int)), this,
            SLOT(handleHorizontalSectionMoved(int, int, int)));
    connect(d_horizontal_header, SIGNAL(sectionDoubleClicked(int)), this,
            SLOT(handleHorizontalHeaderDoubleClicked(int)));

    d_horizontal_header->setDefaultSectionSize(future::Table::defaultColumnWidth());

    v_header->installEventFilter(this);
    d_horizontal_header->installEventFilter(this);
    d_view_widget->installEventFilter(this);

    connect(d_model, SIGNAL(headerDataChanged(Qt::Orientation, int, int)), d_view_widget,
            SLOT(updateHeaderGeometry(Qt::Orientation, int, int)));
    connect(d_model, SIGNAL(headerDataChanged(Qt::Orientation, int, int)), this,
            SLOT(handleHeaderDataChanged(Qt::Orientation, int, int)));
    connect(d_table, SIGNAL(aspectDescriptionChanged(const AbstractAspect *)), this,
            SLOT(handleAspectDescriptionChanged(const AbstractAspect *)));
    connect(d_table, SIGNAL(aspectAdded(const AbstractAspect *)), this,
            SLOT(handleAspectAdded(const AbstractAspect *)));
    connect(d_table, SIGNAL(aspectAboutToBeRemoved(const AbstractAspect *, int)), this,
            SLOT(handleAspectAboutToBeRemoved(const AbstractAspect *, int)));

    this->setWidget(d_main_widget);
    rereadSectionSizes();

    // keyboard shortcuts
    QShortcut *sel_all =
            new QShortcut(QKeySequence(tr("Ctrl+A", "Table: select all")), d_view_widget);
    connect(sel_all, SIGNAL(activated()), d_view_widget, SLOT(selectAll()));

    connect(ui.type_box, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFormatBox()));
    connect(ui.format_box, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTypeInfo()));
    connect(ui.formatLineEdit, SIGNAL(textEdited(const QString)), this,
            SLOT(handleFormatLineEditChange()));
    connect(ui.digits_box, SIGNAL(valueChanged(int)), this, SLOT(updateTypeInfo()));
    connect(ui.previous_column_button, SIGNAL(clicked()), this, SLOT(goToPreviousColumn()));
    connect(ui.next_column_button, SIGNAL(clicked()), this, SLOT(goToNextColumn()));
    retranslateStrings();

    QItemSelectionModel *sel_model = d_view_widget->selectionModel();

    connect(sel_model, SIGNAL(currentColumnChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(currentColumnChanged(const QModelIndex &, const QModelIndex &)));
    connect(sel_model, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(selectionChanged(const QItemSelection &, const QItemSelection &)));
    connect(ui.button_set_description, SIGNAL(pressed()), this, SLOT(applyDescription()));
    connect(ui.button_set_type, SIGNAL(pressed()), this, SLOT(applyType()));
}

void TableView::rereadSectionSizes()
{
    disconnect(d_horizontal_header, SIGNAL(sectionResized(int, int, int)), this,
               SLOT(handleHorizontalSectionResized(int, int, int)));

    if (d_table) {
        int cols = d_table->columnCount();
        for (int i = 0; i < cols; i++)
            d_horizontal_header->resizeSection(i, d_table->columnWidth(i));
    }

    connect(d_horizontal_header, SIGNAL(sectionResized(int, int, int)), this,
            SLOT(handleHorizontalSectionResized(int, int, int)));
}

void TableView::setColumnWidth(int col, int width)
{
    d_horizontal_header->resizeSection(col, width);
}

int TableView::columnWidth(int col) const
{
    return d_horizontal_header->sectionSize(col);
}

void TableView::handleHorizontalSectionResized(int logicalIndex, int, int newSize)
{
    static bool inside = false;
    if (d_table) {
        d_table->setColumnWidth(logicalIndex, newSize);
        if (inside)
            return;
        inside = true;

        int cols = d_table->columnCount();
        for (int i = 0; i < cols; i++)
            if (isColumnSelected(i, true))
                d_horizontal_header->resizeSection(i, newSize);
    }

    inside = false;
}

void TableView::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
        retranslateStrings();
    MyWidget::changeEvent(event);
}

void TableView::retranslateStrings()
{
    d_hide_button->setToolTip(tr("Show/hide control tabs"));
    ui.retranslateUi(d_control_tabs);

    // prevent losing current selection on retranslate
    auto index = ui.type_box->currentIndex();
    if (-1 == index)
        index = 0;
    ui.type_box->clear();
    ui.type_box->addItem(tr("Numeric"), QVariant(int(SciDAVis::ColumnMode::Numeric)));
    ui.type_box->addItem(tr("Text"), QVariant(int(SciDAVis::ColumnMode::Text)));
    ui.type_box->addItem(tr("Month names"), QVariant(int(SciDAVis::ColumnMode::Month)));
    ui.type_box->addItem(tr("Day names"), QVariant(int(SciDAVis::ColumnMode::Day)));
    ui.type_box->addItem(tr("Date and time"), QVariant(int(SciDAVis::ColumnMode::DateTime)));
    ui.type_box->setCurrentIndex(index);

    // prevent losing current selection on retranslate
    index = ui.date_time_interval->currentIndex();
    if (-1 == index)
        index = 0;
    ui.date_time_interval->clear();
    ui.date_time_interval->addItem(tr("years"), int(NumericDateTimeBaseFilter::UnitInterval::Year));
    ui.date_time_interval->addItem(tr("months"),
                                   int(NumericDateTimeBaseFilter::UnitInterval::Month));
    ui.date_time_interval->addItem(tr("days"), int(NumericDateTimeBaseFilter::UnitInterval::Day));
    ui.date_time_interval->addItem(tr("hours"), int(NumericDateTimeBaseFilter::UnitInterval::Hour));
    ui.date_time_interval->addItem(tr("minutes"),
                                   int(NumericDateTimeBaseFilter::UnitInterval::Minute));
    ui.date_time_interval->addItem(tr("seconds"),
                                   int(NumericDateTimeBaseFilter::UnitInterval::Second));
    ui.date_time_interval->addItem(tr("milliseconds"),
                                   int(NumericDateTimeBaseFilter::UnitInterval::Millisecond));
    ui.date_time_interval->setCurrentIndex(index);

    // TODO: implement formula stuff
    // ui.formula_info->document()->setPlainText("not implemented yet");
}

void TableView::advanceCell()
{
    if (d_table) {
        QModelIndex idx = d_view_widget->currentIndex();
        if (idx.row() + 1 >= d_table->rowCount()) {
            int new_size = d_table->rowCount() + 1;
            d_table->setRowCount(new_size);
        }
        d_view_widget->setCurrentIndex(idx.sibling(idx.row() + 1, idx.column()));
    }
}

void TableView::goToCell(int row, int col)
{
    QModelIndex index = d_model->index(row, col);
    d_view_widget->scrollTo(index);
    d_view_widget->setCurrentIndex(index);
}

void TableView::selectAll()
{
    d_view_widget->selectAll();
}

void TableView::deselectAll()
{
    d_view_widget->clearSelection();
}

void TableView::toggleControlTabBar()
{
    d_control_tabs->setVisible(!d_control_tabs->isVisible());
    if (d_control_tabs->isVisible())
        d_hide_button->setArrowType(Qt::RightArrow);
    else
        d_hide_button->setArrowType(Qt::LeftArrow);
}

void TableView::handleHorizontalSectionMoved(int, int from, int to)
{
    static bool inside = false;
    if (inside)
        return;

    inside = true;
    d_view_widget->horizontalHeader()->moveSection(to, from);
    inside = false;
    if (d_table)
        d_table->moveColumn(from, to);
}

void TableView::handleHorizontalHeaderDoubleClicked(int index)
{
    Q_UNUSED(index);
    showControlDescriptionTab();
}

bool TableView::areCommentsShown() const
{
    return d_horizontal_header->areCommentsShown();
}

void TableView::toggleComments()
{
    showComments(!areCommentsShown());
}

void TableView::showComments(bool on)
{
    d_horizontal_header->showComments(on);
    update();
}

void TableView::currentColumnChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    int col = current.column();
    if (col < 0 || (d_table && col >= d_table->columnCount()))
        return;
    setColumnForControlTabs(col);
    d_table->setCurrentColumn(col);
}

void TableView::setColumnForControlTabs(int col)
{
    if (d_table) {
        if (col < 0 || col >= d_table->columnCount())
            return;
        Column *col_ptr = d_table->column(col);

        QString str =
                tr("Current column:\nName: %1\nPosition: %2").arg(col_ptr->name()).arg(col + 1);

        ui.name_edit->setText(col_ptr->name());
        ui.comment_box->document()->setPlainText(col_ptr->comment());
        ui.type_box->setCurrentIndex(ui.type_box->findData((int)col_ptr->columnMode()));
        switch (col_ptr->columnMode()) {
        case SciDAVis::ColumnMode::Numeric: {
            Double2StringFilter *filter =
                    static_cast<Double2StringFilter *>(col_ptr->outputFilter());
            ui.format_box->setCurrentIndex(ui.format_box->findData(filter->numericFormat()));
            ui.digits_box->setValue(filter->numDigits());
            ui.date_time_interval->setVisible(false);
            ui.date_time_interval_label->setVisible(false);
            ui.date_time_0->setVisible(false);
            ui.date_time_0_label->setVisible(false);
            break;
        }
        case SciDAVis::ColumnMode::Month:
        case SciDAVis::ColumnMode::Day:
        case SciDAVis::ColumnMode::DateTime: {
            DateTime2StringFilter *filter =
                    static_cast<DateTime2StringFilter *>(col_ptr->outputFilter());
            ui.formatLineEdit->setText(filter->format());
            ui.format_box->setCurrentIndex(ui.format_box->findData(filter->format()));
            auto num_filter = col_ptr->numericDateTimeBaseFilter();
            ui.date_time_0->setDateTime(num_filter->getBaseDateTime());
            auto unit = static_cast<int>(num_filter->getUnitInterval());
            ui.date_time_interval->setCurrentIndex(ui.date_time_interval->findData(unit));
            break;
        }
        default:
            ui.date_time_interval->setVisible(false);
            ui.date_time_interval_label->setVisible(false);
            ui.date_time_0->setVisible(false);
            ui.date_time_0_label->setVisible(false);
            break;
        }
        ui.formula_box->setText(col_ptr->formula(0));
    }
}

void TableView::handleAspectDescriptionChanged(const AbstractAspect *aspect)
{
    if (d_table) {
        const Column *col = qobject_cast<const Column *>(aspect);
        if (!col || col->parentAspect() != static_cast<AbstractAspect *>(d_table))
            return;
        ui.add_reference_combobox->setItemText(d_table->columnIndex(col),
                                               "col(\"" + col->name() + "\")");
    }
}

void TableView::handleAspectAdded(const AbstractAspect *aspect)
{
    if (d_table) {
        const Column *col = qobject_cast<const Column *>(aspect);
        if (!col || col->parentAspect() != static_cast<AbstractAspect *>(d_table))
            return;
        ui.add_reference_combobox->insertItem(d_table->indexOfChild(aspect),
                                              "col(\"" + col->name() + "\")");
    }
}

void TableView::handleAspectAboutToBeRemoved(const AbstractAspect *parent, int index)
{
    if (parent != d_table)
        return;
    ui.add_reference_combobox->removeItem(index);
}

void TableView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);
}

void TableView::updateFormatBox()
{
    int type_index = ui.type_box->currentIndex();
    if (type_index < 0)
        return; // should never happen
    ui.format_box->clear();
    ui.digits_box->setEnabled(false);
    ui.formatLineEdit->setEnabled(false);
    ui.date_time_interval->setEnabled(false);
    ui.date_time_0->setEnabled(false);
    switch (static_cast<SciDAVis::ColumnMode>(ui.type_box->itemData(type_index).toInt())) {
    case SciDAVis::ColumnMode::Numeric:
        ui.digits_box->setEnabled(true);
        ui.format_box->addItem(tr("Decimal"), QVariant('f'));
        ui.format_box->addItem(tr("Scientific (e)"), QVariant('e'));
        ui.format_box->addItem(tr("Scientific (E)"), QVariant('E'));
        ui.format_box->addItem(tr("Automatic (e)"), QVariant('g'));
        ui.format_box->addItem(tr("Automatic (E)"), QVariant('G'));
        break;
    case SciDAVis::ColumnMode::Text:
        ui.format_box->addItem(tr("Text"), QVariant());
        break;
    case SciDAVis::ColumnMode::Month:
        ui.format_box->addItem(tr("Number without leading zero"), QVariant("M"));
        ui.format_box->addItem(tr("Number with leading zero"), QVariant("MM"));
        ui.format_box->addItem(tr("Abbreviated month name"), QVariant("MMM"));
        ui.format_box->addItem(tr("Full month name"), QVariant("MMMM"));
        break;
    case SciDAVis::ColumnMode::Day:
        ui.format_box->addItem(tr("Number without leading zero"), QVariant("d"));
        ui.format_box->addItem(tr("Number with leading zero"), QVariant("dd"));
        ui.format_box->addItem(tr("Abbreviated day name"), QVariant("ddd"));
        ui.format_box->addItem(tr("Full day name"), QVariant("dddd"));
        break;
    case SciDAVis::ColumnMode::DateTime: {
        // TODO: allow adding of the combo box entries here
        const char *date_strings[] = {
            "yyyy-MM-dd", "yyyy/MM/dd", "dd/MM/yyyy", "dd/MM/yy", "dd.MM.yyyy",
            "dd.MM.yy",   "MM/yyyy",    "dd.MM.",     "yyyyMMdd", 0
        };

        const char *time_strings[] = {
            "hh",           "hh ap",        "hh:mm",     "hh:mm ap", "hh:mm:ss",
            "hh:mm:ss.zzz", "hh:mm:ss:zzz", "mm:ss.zzz", "hhmmss",   0
        };
        int j, i;
        for (i = 0; date_strings[i] != 0; i++)
            ui.format_box->addItem(QString(date_strings[i]), QVariant(date_strings[i]));
        for (j = 0; time_strings[j] != 0; j++)
            ui.format_box->addItem(QString(time_strings[j]), QVariant(time_strings[j]));
        for (i = 0; date_strings[i] != 0; i++)
            for (j = 0; time_strings[j] != 0; j++)
                ui.format_box->addItem(
                        QString("%1 %2").arg(date_strings[i]).arg(time_strings[j]),
                        QVariant(QString(date_strings[i]) + " " + QString(time_strings[j])));
        ui.formatLineEdit->setEnabled(true);
        ui.date_time_interval->setEnabled(true);
        ui.date_time_0->setEnabled(true);
        break;
    }
    default:
        ui.format_box->addItem(QString()); // just for savety to have at least one item in any case
    }
    ui.format_box->setCurrentIndex(0);
    ui.digits_box->setVisible(ui.digits_box->isEnabled());
    ui.digits_label->setVisible(ui.digits_box->isEnabled());
    ui.formatLineEdit->setVisible(ui.formatLineEdit->isEnabled());
    ui.format_label2->setVisible(ui.formatLineEdit->isEnabled());
    if (ui.format_label2->isVisible())
        ui.format_label->setText(tr("Predefined:"));
    else
        ui.format_label->setText(tr("Format:"));
    ui.date_time_interval->setVisible(ui.date_time_interval->isEnabled());
    ui.date_time_interval_label->setVisible(ui.date_time_interval->isEnabled());
    ui.date_time_0->setVisible(ui.date_time_0->isEnabled());
    ui.date_time_0_label->setVisible(ui.date_time_0->isEnabled());
}

void TableView::updateTypeInfo()
{
    int format_index = ui.format_box->currentIndex();
    int type_index = ui.type_box->currentIndex();

    QString str = tr("Selected column type:\n");
    if (format_index >= 0 && type_index >= 0) {
        SciDAVis::ColumnMode type = static_cast<SciDAVis::ColumnMode>(ui.type_box->itemData(type_index).toInt());
        switch (type) {
        case SciDAVis::ColumnMode::Numeric:
            str += tr("Double precision\nfloating point values\n");
            ui.digits_box->setEnabled(true);
            break;
        case SciDAVis::ColumnMode::Text:
            str += tr("Text\n");
            break;
        case SciDAVis::ColumnMode::Month:
            str += tr("Month names\n");
            break;
        case SciDAVis::ColumnMode::Day:
            str += tr("Days of the week\n");
            break;
        case SciDAVis::ColumnMode::DateTime:
            str += tr("Dates and/or times\n");
            ui.formatLineEdit->setEnabled(true);
            break;
        }
        str += tr("Example: ");
        switch (type) {
        case SciDAVis::ColumnMode::Numeric:
            str += QString::number(123.1234567890123456,
                                   ui.format_box->itemData(format_index).toChar().toLatin1(),
                                   ui.digits_box->value());
            break;
        case SciDAVis::ColumnMode::Text:
            str += tr("Hello world!\n");
            break;
        case SciDAVis::ColumnMode::Month:
            str += QLocale().toString(QDate(1900, 1, 1),
                                      ui.format_box->itemData(format_index).toString());
            break;
        case SciDAVis::ColumnMode::Day:
            str += QLocale().toString(QDate(1900, 1, 1),
                                      ui.format_box->itemData(format_index).toString());
            break;
        case SciDAVis::ColumnMode::DateTime:
            ui.formatLineEdit->setText(ui.format_box->itemData(format_index).toString());
            ui.date_time_0->setDisplayFormat(ui.format_box->itemData(format_index).toString());
            str += QDateTime(QDate(1900, 1, 1), QTime(23, 59, 59, 999))
                           .toString(ui.formatLineEdit->text());
            break;
        }
    } else if (format_index == -1 && type_index >= 0
               && static_cast<SciDAVis::ColumnMode>(ui.type_box->itemData(type_index).toInt())
                       == SciDAVis::ColumnMode::DateTime) {
        str += tr("Dates and/or times\n");
        ui.formatLineEdit->setEnabled(true);
        str += tr("Example: ");
        str += QDateTime(QDate(1900, 1, 1), QTime(23, 59, 59, 999))
                       .toString(ui.formatLineEdit->text());
    }

    ui.type_info->setText(str);
    ui.digits_box->setVisible(ui.digits_box->isEnabled());
    ui.digits_label->setVisible(ui.digits_box->isEnabled());
    ui.formatLineEdit->setVisible(ui.formatLineEdit->isEnabled());
    ui.format_label2->setVisible(ui.formatLineEdit->isEnabled());
    if (ui.format_label2->isVisible())
        ui.format_label->setText(tr("Predefined:"));
    else
        ui.format_label->setText(tr("Format:"));
}

void TableView::handleFormatLineEditChange()
{
    int type_index = ui.type_box->currentIndex();

    if (type_index >= 0) {
        SciDAVis::ColumnMode type = static_cast<SciDAVis::ColumnMode>(ui.type_box->itemData(type_index).toInt());
        if (type == SciDAVis::ColumnMode::DateTime) {
            QString str = tr("Selected column type:\n");
            str += tr("Dates and/or times\n");
            str += tr("Example: ");
            str += QDateTime(QDate(1900, 1, 1), QTime(23, 59, 59, 999))
                           .toString(ui.formatLineEdit->text());
            ui.type_info->setText(str);
            ui.date_time_0->setDisplayFormat(ui.formatLineEdit->text());
        }
    }
}

void TableView::showControlDescriptionTab()
{
    d_control_tabs->setVisible(true);
    d_hide_button->setArrowType(Qt::RightArrow);
    ui.tab_widget->setCurrentIndex(0);
    ui.tab_widget->setFocus();
}

void TableView::showControlTypeTab()
{
    d_control_tabs->setVisible(true);
    d_hide_button->setArrowType(Qt::RightArrow);
    ui.tab_widget->setCurrentIndex(1);
    ui.tab_widget->setFocus();
}

void TableView::showControlFormulaTab()
{
    d_control_tabs->setVisible(true);
    d_hide_button->setArrowType(Qt::RightArrow);
    ui.tab_widget->setCurrentIndex(2);
    ui.tab_widget->setFocus();
}

void TableView::applyDescription()
{
    QItemSelectionModel *sel_model = d_view_widget->selectionModel();
    int index = sel_model->currentIndex().column();
    if (d_table && index >= 0) {
        // changing the name triggers an update of the UI, which also resets the content of the
        // comment box => need to cache it so name and comment can be changed simultaneously
        QString comment = ui.comment_box->document()->toPlainText();
        d_table->column(index)->setName(ui.name_edit->text());
        d_table->column(index)->setComment(comment);
    }
}

void TableView::applyType()
{
    int format_index = ui.format_box->currentIndex();
    int type_index = ui.type_box->currentIndex();
    if (format_index < 0 && type_index < 0)
        return;

    SciDAVis::ColumnMode new_mode = (SciDAVis::ColumnMode)ui.type_box->itemData(type_index).toInt();
    QList<Column *> list = selectedColumns();
    if ((0 == list.size()) && (nullptr != d_table->currentColumn()))
        list.append(d_table->currentColumn());
    switch (new_mode) {
    case SciDAVis::ColumnMode::Numeric: {
        foreach (Column *col, list) {
            col->beginMacro(QObject::tr("%1: change column type").arg(col->name()));
            col->setColumnMode(new_mode);
            Double2StringFilter *filter = static_cast<Double2StringFilter *>(col->outputFilter());
            int digits =
                    ui.digits_box->value(); // setNumericFormat causes digits_box to be modified...
            filter->setNumericFormat(ui.format_box->itemData(format_index).toChar().toLatin1());
            filter->setNumDigits(digits);
            col->endMacro();
        }
        break;
    }
    case SciDAVis::ColumnMode::Text:
        foreach (Column *col, list)
            col->setColumnMode(new_mode);
        break;
    case SciDAVis::ColumnMode::Month:
    case SciDAVis::ColumnMode::Day:
    case SciDAVis::ColumnMode::DateTime:
        QString format;
        if (ui.formatLineEdit->isEnabled())
            format = ui.formatLineEdit->text();
        else
            format = ui.format_box->itemData(format_index).toString();
        foreach (Column *col, list) {
            col->beginMacro(QObject::tr("%1: change column type").arg(col->name()));
            SciDAVis::ColumnMode old_mode = col->columnMode();
            AbstractFilter *converter = 0;
            switch (old_mode) {
            case SciDAVis::ColumnMode::Numeric: // the mode is changed
            case SciDAVis::ColumnMode::DateTime: // the mode is not changed, but numeric converter parameters is
                                     // (possibly) changed
                if (ui.date_time_interval->isVisible()) {
                    Double2DateTimeFilter::UnitInterval unit =
                            (Double2DateTimeFilter::UnitInterval)ui.date_time_interval
                                    ->itemData(ui.date_time_interval->currentIndex())
                                    .toInt();
                    QDateTime date_time_0 = ui.date_time_0->dateTime();
                    converter = new Double2DateTimeFilter(unit, date_time_0);
                }
                break;
            case SciDAVis::ColumnMode::Text:
                converter = new String2DateTimeFilter(format);
                break;
            default:
                break;
            }
            col->setColumnMode(new_mode, converter);
            {
                auto filter = dynamic_cast<DateTime2StringFilter *>(col->outputFilter());
                if (nullptr != filter)
                    filter->setFormat(format);
            }
            {
                auto filter = dynamic_cast<String2DateTimeFilter *>(col->inputFilter());
                if (nullptr != filter)
                    filter->setFormat(format);
            }
            col->endMacro();
        }
        break;
    }
}

void TableView::handleHeaderDataChanged(Qt::Orientation orientation, int first, int last)
{
    if (orientation != Qt::Horizontal)
        return;

    QItemSelectionModel *sel_model = d_view_widget->selectionModel();

    int col = sel_model->currentIndex().column();
    if (col < first || col > last)
        return;
    setColumnForControlTabs(col);
}

int TableView::selectedColumnCount(bool full)
{
    int count = 0;
    if (d_table) {
        int cols = d_table->columnCount();
        for (int i = 0; i < cols; i++)
            if (isColumnSelected(i, full))
                count++;
    }
    return count;
}

int TableView::selectedColumnCount(SciDAVis::PlotDesignation pd)
{
    int count = 0;
    if (d_table) {
        int cols = d_table->columnCount();
        for (int i = 0; i < cols; i++)
            if (isColumnSelected(i, false) && (d_table->column(i)->plotDesignation() == pd))
                count++;
    }
    return count;
}

bool TableView::isColumnSelected(int col, bool full)
{
    if (full)
        return d_view_widget->selectionModel()->isColumnSelected(col, QModelIndex());
    else
        return d_view_widget->selectionModel()->columnIntersectsSelection(col, QModelIndex());
}

QList<Column *> TableView::selectedColumns(bool full)
{
    QList<Column *> list;
    if (d_table) {
        int cols = d_table->columnCount();
        for (int i = 0; i < cols; i++)
            if (isColumnSelected(i, full))
                list << d_table->column(i);
    }
    return list;
}

int TableView::selectedRowCount(bool full)
{
    int count = 0;
    if (d_table) {
        int rows = d_table->rowCount();
        for (int i = 0; i < rows; i++)
            if (isRowSelected(i, full))
                count++;
    }
    return count;
}

bool TableView::isRowSelected(int row, bool full)
{
    if (full)
        return d_view_widget->selectionModel()->isRowSelected(row, QModelIndex());
    else
        return d_view_widget->selectionModel()->rowIntersectsSelection(row, QModelIndex());
}

int TableView::firstSelectedColumn(bool full)
{
    if (d_table) {
        int cols = d_table->columnCount();
        for (int i = 0; i < cols; i++) {
            if (isColumnSelected(i, full))
                return i;
        }
    }
    return -1;
}

int TableView::lastSelectedColumn(bool full)
{
    if (d_table) {
        int cols = d_table->columnCount();
        for (int i = cols - 1; i >= 0; i--)
            if (isColumnSelected(i, full))
                return i;
    }
    return -2;
}

int TableView::firstSelectedRow(bool full)
{
    if (d_table) {
        int rows = d_table->rowCount();
        for (int i = 0; i < rows; i++) {
            if (isRowSelected(i, full))
                return i;
        }
    }
    return -1;
}

int TableView::lastSelectedRow(bool full)
{
    if (d_table) {
        int rows = d_table->rowCount();
        for (int i = rows - 1; i >= 0; i--)
            if (isRowSelected(i, full))
                return i;
    }
    return -2;
}

IntervalAttribute<bool> TableView::selectedRows(bool full)
{
    IntervalAttribute<bool> result;
    if (d_table) {
        int rows = d_table->rowCount();
        for (int i = 0; i < rows; i++)
            if (isRowSelected(i, full))
                result.setValue(i, true);
    }
    return result;
}

bool TableView::hasMultiSelection()
{
    QModelIndexList indexes = d_view_widget->selectionModel()->selectedIndexes();
    if (indexes.size() < 2)
        return false;

    QModelIndex index;
    int minrow = indexes.at(0).row();
    int maxrow = minrow;
    int mincol = indexes.at(0).column();
    int maxcol = mincol;
    foreach (index, indexes) {
        minrow = std::min(minrow, index.row());
        maxrow = std::max(maxrow, index.row());
        mincol = std::min(mincol, index.column());
        maxcol = std::max(maxcol, index.column());
    }
    int spanned = (maxrow - minrow + 1) * (maxcol - mincol + 1);
    return (spanned > d_view_widget->selectionModel()->selectedIndexes().size());
}

bool TableView::isCellSelected(int row, int col)
{
    if (!d_table || row < 0 || col < 0 || row >= d_table->rowCount()
        || col >= d_table->columnCount())
        return false;

    return d_view_widget->selectionModel()->isSelected(d_model->index(row, col));
}

void TableView::setCellSelected(int row, int col, bool select)
{
    d_view_widget->selectionModel()->select(d_model->index(row, col),
                                            select ? QItemSelectionModel::Select
                                                   : QItemSelectionModel::Deselect);
}

void TableView::setCellsSelected(int first_row, int first_col, int last_row, int last_col,
                                 bool select)
{
    QModelIndex top_left = d_model->index(first_row, first_col);
    QModelIndex bottom_right = d_model->index(last_row, last_col);
    d_view_widget->selectionModel()->select(QItemSelection(top_left, bottom_right),
                                            select ? QItemSelectionModel::SelectCurrent
                                                   : QItemSelectionModel::Deselect);
}

void TableView::getCurrentCell(int *row, int *col)
{
    QModelIndex index = d_view_widget->selectionModel()->currentIndex();
    if (index.isValid()) {
        *row = index.row();
        *col = index.column();
    } else {
        *row = -1;
        *col = -1;
    }
}

bool TableView::eventFilter(QObject *watched, QEvent *event)
{
    QHeaderView *v_header = d_view_widget->verticalHeader();

    if (d_table && event->type() == QEvent::ContextMenu) {
        QContextMenuEvent *cm_event = static_cast<QContextMenuEvent *>(event);
        QPoint global_pos = cm_event->globalPos();
        if (watched == v_header)
            d_table->showTableViewRowContextMenu(global_pos);
        else if (watched == d_horizontal_header) {
            int col = d_horizontal_header->logicalIndexAt(cm_event->pos());
            if (!isColumnSelected(col, true)) {
                QItemSelectionModel *sel_model = d_view_widget->selectionModel();
                sel_model->clearSelection();
                sel_model->select(
                        QItemSelection(d_model->index(0, col, QModelIndex()),
                                       d_model->index(d_model->rowCount() - 1, col, QModelIndex())),
                        QItemSelectionModel::Select);
            }
            d_table->showTableViewColumnContextMenu(global_pos);
        } else if (watched == d_view_widget)
            d_table->showTableViewContextMenu(global_pos);
        else
            return MyWidget::eventFilter(watched, event);

        return true;
    } else
        return MyWidget::eventFilter(watched, event);
}

bool TableView::formulaModeActive() const
{
    return d_model->formulaModeActive();
}

void TableView::activateFormulaMode(bool on)
{
    d_model->activateFormulaMode(on);
}

void TableView::goToNextColumn()
{
    if (!d_table || d_table->columnCount() == 0)
        return;

    QModelIndex idx = d_view_widget->currentIndex();
    int col = idx.column() + 1;
    if (col >= d_table->columnCount())
        col = 0;
    d_view_widget->setCurrentIndex(idx.sibling(idx.row(), col));
}

void TableView::goToPreviousColumn()
{
    if (!d_table || d_table->columnCount() == 0)
        return;

    QModelIndex idx = d_view_widget->currentIndex();
    int col = idx.column() - 1;
    if (col < 0)
        col = d_table->columnCount() - 1;
    d_view_widget->setCurrentIndex(idx.sibling(idx.row(), col));
}

/* ================== TableViewWidget ================ */

void TableViewWidget::selectAll()
{
    // the original QTableView::selectAll() toggles all cells which is strange behavior IMHO - thzs
    QItemSelectionModel *sel_model = selectionModel();
    QItemSelection sel(
            model()->index(0, 0, QModelIndex()),
            model()->index(model()->rowCount() - 1, model()->columnCount() - 1, QModelIndex()));
    sel_model->select(sel, QItemSelectionModel::Select);
}

void TableViewWidget::updateHeaderGeometry(Qt::Orientation o, int first, int last)
{
    Q_UNUSED(first)
    Q_UNUSED(last)
    if (o != Qt::Horizontal)
        return;
    horizontalHeader()->setStretchLastSection(
            true); // ugly hack (flaw in Qt? Does anyone know a better way?)
    horizontalHeader()->updateGeometry();
    horizontalHeader()->setStretchLastSection(false); // ugly hack part 2
}

void TableViewWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
        emit advanceCell();
    QTableView::keyPressEvent(event);
}
