find_package (PythonInterp)

if (NOT PYTHONINTERP_FOUND)
    message (FATAL_ERROR "Python was not found")
endif ()

# Check if python module 'module' is available and store result in 'var'
function (find_python_module module var)
    execute_process (COMMAND ${PYTHON_EXECUTABLE} -c "import imp; imp.find_module('${module}')"
        RESULT_VARIABLE result)
    if (result EQUAL 0)
        set (${var} TRUE PARENT_SCOPE)
    else ()
        set (${var} FALSE PARENT_SCOPE)
    endif ()
endfunction ()
