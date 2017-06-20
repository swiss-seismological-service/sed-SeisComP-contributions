# This file was automatically generated by SWIG (http://www.swig.org).
# Version 3.0.12
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.

from sys import version_info as _swig_python_version_info
if _swig_python_version_info >= (2, 7, 0):
    def swig_import_helper():
        import importlib
        pkg = __name__.rpartition('.')[0]
        mname = '.'.join((pkg, '_Logging')).lstrip('.')
        try:
            return importlib.import_module(mname)
        except ImportError:
            return importlib.import_module('_Logging')
    _Logging = swig_import_helper()
    del swig_import_helper
elif _swig_python_version_info >= (2, 6, 0):
    def swig_import_helper():
        from os.path import dirname
        import imp
        fp = None
        try:
            fp, pathname, description = imp.find_module('_Logging', [dirname(__file__)])
        except ImportError:
            import _Logging
            return _Logging
        try:
            _mod = imp.load_module('_Logging', fp, pathname, description)
        finally:
            if fp is not None:
                fp.close()
        return _mod
    _Logging = swig_import_helper()
    del swig_import_helper
else:
    import _Logging
del _swig_python_version_info

try:
    _swig_property = property
except NameError:
    pass  # Python < 2.2 doesn't have 'property'.

try:
    import builtins as __builtin__
except ImportError:
    import __builtin__

def _swig_setattr_nondynamic(self, class_type, name, value, static=1):
    if (name == "thisown"):
        return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'SwigPyObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name, None)
    if method:
        return method(self, value)
    if (not static):
        if _newclass:
            object.__setattr__(self, name, value)
        else:
            self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)


def _swig_setattr(self, class_type, name, value):
    return _swig_setattr_nondynamic(self, class_type, name, value, 0)


def _swig_getattr(self, class_type, name):
    if (name == "thisown"):
        return self.this.own()
    method = class_type.__swig_getmethods__.get(name, None)
    if method:
        return method(self)
    raise AttributeError("'%s' object has no attribute '%s'" % (class_type.__name__, name))


def _swig_repr(self):
    try:
        strthis = "proxy of " + self.this.__repr__()
    except __builtin__.Exception:
        strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

try:
    _object = object
    _newclass = 1
except __builtin__.Exception:
    class _object:
        pass
    _newclass = 0

SEISCOMP_COMPONENT = _Logging.SEISCOMP_COMPONENT
LL_UNDEFINED = _Logging.LL_UNDEFINED
LL_CRITICAL = _Logging.LL_CRITICAL
LL_ERROR = _Logging.LL_ERROR
LL_WARNING = _Logging.LL_WARNING
LL_NOTICE = _Logging.LL_NOTICE
LL_INFO = _Logging.LL_INFO
LL_DEBUG = _Logging.LL_DEBUG
LL_QUANTITY = _Logging.LL_QUANTITY

def debug(arg1):
    return _Logging.debug(arg1)
debug = _Logging.debug

def info(arg1):
    return _Logging.info(arg1)
info = _Logging.info

def warning(arg1):
    return _Logging.warning(arg1)
warning = _Logging.warning

def error(arg1):
    return _Logging.error(arg1)
error = _Logging.error

def notice(arg1):
    return _Logging.notice(arg1)
notice = _Logging.notice

def log(arg1, format):
    return _Logging.log(arg1, format)
log = _Logging.log

def getAll():
    return _Logging.getAll()
getAll = _Logging.getAll

def getGlobalChannel(*args):
    return _Logging.getGlobalChannel(*args)
getGlobalChannel = _Logging.getGlobalChannel

def getComponentChannel(*args):
    return _Logging.getComponentChannel(*args)
getComponentChannel = _Logging.getComponentChannel

def getComponentAll(component):
    return _Logging.getComponentAll(component)
getComponentAll = _Logging.getComponentAll

def getComponentDebugs(component):
    return _Logging.getComponentDebugs(component)
getComponentDebugs = _Logging.getComponentDebugs

def getComponentInfos(component):
    return _Logging.getComponentInfos(component)
getComponentInfos = _Logging.getComponentInfos

def getComponentWarnings(component):
    return _Logging.getComponentWarnings(component)
getComponentWarnings = _Logging.getComponentWarnings

def getComponentErrors(component):
    return _Logging.getComponentErrors(component)
getComponentErrors = _Logging.getComponentErrors

def getComponentNotices(component):
    return _Logging.getComponentNotices(component)
getComponentNotices = _Logging.getComponentNotices

def consoleOutput():
    return _Logging.consoleOutput()
consoleOutput = _Logging.consoleOutput

def enableConsoleLogging(arg1):
    return _Logging.enableConsoleLogging(arg1)
enableConsoleLogging = _Logging.enableConsoleLogging

def disableConsoleLogging():
    return _Logging.disableConsoleLogging()
disableConsoleLogging = _Logging.disableConsoleLogging

def init(argc, argv):
    return _Logging.init(argc, argv)
init = _Logging.init
class Output(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Output, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Output, name)

    def __init__(self, *args, **kwargs):
        raise AttributeError("No constructor defined - class is abstract")
    __repr__ = _swig_repr
    __swig_destroy__ = _Logging.delete_Output
    __del__ = lambda self: None

    def subscribe(self, channel):
        return _Logging.Output_subscribe(self, channel)

    def unsubscribe(self, channel):
        return _Logging.Output_unsubscribe(self, channel)

    def logComponent(self, e):
        return _Logging.Output_logComponent(self, e)

    def logContext(self, e):
        return _Logging.Output_logContext(self, e)

    def setUTCEnabled(self, e):
        return _Logging.Output_setUTCEnabled(self, e)
Output_swigregister = _Logging.Output_swigregister
Output_swigregister(Output)

class FdOutput(Output):
    __swig_setmethods__ = {}
    for _s in [Output]:
        __swig_setmethods__.update(getattr(_s, '__swig_setmethods__', {}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, FdOutput, name, value)
    __swig_getmethods__ = {}
    for _s in [Output]:
        __swig_getmethods__.update(getattr(_s, '__swig_getmethods__', {}))
    __getattr__ = lambda self, name: _swig_getattr(self, FdOutput, name)
    __repr__ = _swig_repr

    def __init__(self, fdOut=2):
        this = _Logging.new_FdOutput(fdOut)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this
    __swig_destroy__ = _Logging.delete_FdOutput
    __del__ = lambda self: None
FdOutput_swigregister = _Logging.FdOutput_swigregister
FdOutput_swigregister(FdOutput)

class FileOutput(Output):
    __swig_setmethods__ = {}
    for _s in [Output]:
        __swig_setmethods__.update(getattr(_s, '__swig_setmethods__', {}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, FileOutput, name, value)
    __swig_getmethods__ = {}
    for _s in [Output]:
        __swig_getmethods__.update(getattr(_s, '__swig_getmethods__', {}))
    __getattr__ = lambda self, name: _swig_getattr(self, FileOutput, name)
    __repr__ = _swig_repr

    def __init__(self, *args):
        this = _Logging.new_FileOutput(*args)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this
    __swig_destroy__ = _Logging.delete_FileOutput
    __del__ = lambda self: None

    def open(self, filename):
        return _Logging.FileOutput_open(self, filename)

    def isOpen(self):
        return _Logging.FileOutput_isOpen(self)
FileOutput_swigregister = _Logging.FileOutput_swigregister
FileOutput_swigregister(FileOutput)

class FileRotatorOutput(FileOutput):
    __swig_setmethods__ = {}
    for _s in [FileOutput]:
        __swig_setmethods__.update(getattr(_s, '__swig_setmethods__', {}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, FileRotatorOutput, name, value)
    __swig_getmethods__ = {}
    for _s in [FileOutput]:
        __swig_getmethods__.update(getattr(_s, '__swig_getmethods__', {}))
    __getattr__ = lambda self, name: _swig_getattr(self, FileRotatorOutput, name)
    __repr__ = _swig_repr

    def __init__(self, *args):
        this = _Logging.new_FileRotatorOutput(*args)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this

    def open(self, filename):
        return _Logging.FileRotatorOutput_open(self, filename)
    __swig_destroy__ = _Logging.delete_FileRotatorOutput
    __del__ = lambda self: None
FileRotatorOutput_swigregister = _Logging.FileRotatorOutput_swigregister
FileRotatorOutput_swigregister(FileRotatorOutput)

class SyslogOutput(Output):
    __swig_setmethods__ = {}
    for _s in [Output]:
        __swig_setmethods__.update(getattr(_s, '__swig_setmethods__', {}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, SyslogOutput, name, value)
    __swig_getmethods__ = {}
    for _s in [Output]:
        __swig_getmethods__.update(getattr(_s, '__swig_getmethods__', {}))
    __getattr__ = lambda self, name: _swig_getattr(self, SyslogOutput, name)
    __repr__ = _swig_repr

    def __init__(self, *args):
        this = _Logging.new_SyslogOutput(*args)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this
    __swig_destroy__ = _Logging.delete_SyslogOutput
    __del__ = lambda self: None

    def facility(self):
        return _Logging.SyslogOutput_facility(self)

    def open(self, ident, facility=None):
        return _Logging.SyslogOutput_open(self, ident, facility)

    def isOpen(self):
        return _Logging.SyslogOutput_isOpen(self)

    def close(self):
        return _Logging.SyslogOutput_close(self)
SyslogOutput_swigregister = _Logging.SyslogOutput_swigregister
SyslogOutput_swigregister(SyslogOutput)

# This file is compatible with both classic and new-style classes.


