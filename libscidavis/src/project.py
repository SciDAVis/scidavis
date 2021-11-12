"""The build configuration file for SciDAVis, used by sip."""

from sipbuild import Option
from pyqtbuild import PyQtBindings, PyQtProject


class SciDAVisProject(PyQtProject):
    """The SciDAVis Project class."""

    def __init__(self):
        """ Initialize the project. """
        super().__init__()
        self.bindings_factories = [SciDAVisBindings]

    def get_options(self):
        """ Return the list of configurable options. """
        options = super().get_options()
        options.append(
                Option('include_dirs', option_type=list,
                        help="additional directory to search for .sip files",
                        metavar="DIR"))
        return options

    def apply_user_defaults(self, tool):
        """ Set default values for user options that haven't been set yet. """
        super().apply_user_defaults(tool)
        if self.include_dirs is not None:
            self.sip_include_dirs += self.include_dirs


class SciDAVisBindings(PyQtBindings):
    """The SciDAVis Bindings class."""

    def __init__(self, project):
        super().__init__(project, name='scidavis',
                         sip_file='scidavis.sip',
                         qmake_QT=['widgets'])
