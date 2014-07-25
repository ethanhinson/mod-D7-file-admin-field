<?php
//Form callback
function file_admin_field_admin_form($form, &$form_state) {
  $form['#submit'][] = 'file_admin_field_admin_submit';
  $form['#attributes'] = array('enctype' => 'multipart/form-data');
  $form['admin_file'] = array(
    '#type' => 'file',
    '#title' => t('File to upload'),
    '#description' => t('The largest file size you can import is !size',
      array(
        '!size' => format_size(file_upload_max_size())
      )
    ),
  );
  if(variable_get('file_admin_field_upload_path', FALSE)) {
    $title = t('Current File');
    $form['file_location'] = array(
      '#type' => 'markup',
      '#prefix' => '<div class="form-item"><label>'.$title.'</label>',
      '#markup' => variable_get('file_admin_field_upload_path'),
      '#suffix' => '</div>',
    );
  }

    
  // Override page title so it's actually meaningful.
  drupal_set_title('File Upload');
  return system_settings_form($form);
}

/**
 * Submit callback for the WordPress import form.
 */
function file_admin_field_admin_submit($form, &$form_state) {
  if ($_FILES['files']['error']['admin_file']) {
    form_set_error('admin_file', t('The file could not be uploaded, most likely
      because the file size exceeds the configured limit of !filesize',
      array('!filesize' => format_size(file_upload_max_size()))));
    return;
  }
  $directory = 'public://';
  if (!file_prepare_directory($directory, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS)) {
    form_set_error('admin_file', t('Could not prepare directory %directory',
      array('%directory' => $directory)));
    return;
  }

  $tmpfile = $_FILES['files']['tmp_name']['admin_file'];
  if ($tmpfile) {
    // Handle uploaded file
    $filename = $_FILES['files']['name']['admin_file'];
    $destination = $directory . str_replace(' ', '%20', $filename);
    $saved = file_unmanaged_move($tmpfile, $destination, FILE_EXISTS_REPLACE);
    if (!$saved) {
      form_set_error('admin_file', t('Failed to save file to %filename', array('%filename' => $destination)));
      return;
    } else {
      // Finally, store the upload path for registering migrations
      variable_set('file_admin_field_upload_path', $saved);
    }
  }
}