# Configure script from source to target and add execute permissions
function (configure_script source target)

    # Define target directory and filename
    get_filename_component (target_dir ${target} DIRECTORY)
    get_filename_component (target_name ${target} NAME)

    # Define temporary directory and filename
    set (tmp_dir ${target_dir})
    string (APPEND tmp_dir "/tmp")

    set (tmp_file ${tmp_dir})
    string (APPEND tmp_file "/" ${target_name})

    # Configure script
    configure_file (${source} ${tmp_file})

    # Copy temporary file to target with execution permissions
    file (COPY ${tmp_file} DESTINATION ${target_dir}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
        GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

    # Remove temporary directory
    file (REMOVE_RECURSE ${tmp_dir})
endfunction ()
