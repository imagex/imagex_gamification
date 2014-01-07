<?php
/**
 * @file
 * Features UUID integration for BEAN instances.
 */

/**
 * Implements hook_features_export_options().
 */
function uuid_badge_features_export_options() {
  $options = array();
  $query = db_select('imagex_userbadge', 'n');
  $query->fields('n', array('badge_id', 'name', 'uuid'))
    ->orderBy('badge_id', 'ASC');
  $badges = $query->execute()->fetchAll();
  foreach ($badges as $badge) {
    $options[$badge->uuid] = t('@name', array(
      '@name' => $badge->name,
    ));
  }
  return $options;
}

/**
 * Implements hook_features_export().
 */
function uuid_badge_features_export($data, &$export, $module_name = '') {
  $pipe = array();

  $export['dependencies']['imagex_userbadge'] = 'imagex_userbadge';
  $export['dependencies']['uuid_features'] = 'uuid_features';

  uuid_features_load_module_includes();

  $bids = entity_get_id_by_uuid('imagex_userbadge', $data);
  foreach ($bids as $uuid => $bid) {
    // Load the imagex_userbadge matching the $bid.
    $query = new EntityFieldQuery();
    $imagex_userbadge = $query
      // We just want one imagex_userbadge: the one matching
      // the current $bid.
      ->entityCondition('entity_type', 'imagex_userbadge')
      ->propertyCondition('badge_id', $bid)
      ->range(0, 1)
      ->execute();

    $export['features']['uuid_badge'][$uuid] = $uuid;
    $pipe['imagex_userbadge'][$imagex_userbadge['imagex_userbadge'][$bid]->type] = $imagex_userbadge['imagex_userbadge'][$bid]->type;

    // drupal_alter() normally supports just one byref parameter. Using
    // the __drupal_alter_by_ref key, we can store any additional parameters
    // that need to be altered, and they'll be split out into additional params
    // for the hook_*_alter() implementations.  The hook_alter signature is
    // hook_uuid_badge_features_export_alter(&$export, &$pipe, $imagex_userbadge).
    $data = &$export;
    $data['__drupal_alter_by_ref'] = array(&$pipe);
    drupal_alter('uuid_badge_features_export', $data, $imagex_userbadge);
  }

  return $pipe;
}

/**
 * Implements hook_features_export_render().
 */
function uuid_badge_features_export_render($module, $data) {
  $translatables = $code = array();

  uuid_features_load_module_includes();

  $code[] = '  $imagex_userbadges = array();';
  $code[] = '';
  foreach ($data as $uuid) {
    $imagex_userbadges = entity_uuid_load('imagex_userbadge', array($uuid), array(), TRUE);
    if (!count($imagex_userbadges)) {
      continue;
    }
    $first_imagex_userbadge = reset($imagex_userbadges);
    $export = clone $first_imagex_userbadge;

    // Unset some things that dont need to be exported.
    unset($export->bid);
    unset($export->rdf_mapping);
    unset($export->changed);
    unset($export->vid);

    // Enable file field exports.
    uuid_badge_file_field_export($export, 'imagex_userbadge');

    // Allow other modules to alter the export rendering.
    // The hook_alter signature is:
    // hook_uuid_badge_features_export_render_alter(&$export, $imagex_userbadge, $module);
    drupal_alter('uuid_badge_features_export_render', $export, $imagex_userbadge, $module);

    $json = json_encode($export);
    $export_array = json_decode($json, TRUE);

    $code[] = '  $imagex_userbadges[] = ' . features_var_export($export_array, '  ') . ';';
  }

  if (!empty($translatables)) {
    $code[] = features_translatables_export($translatables, '  ');
  }

  $code[] = '  return $imagex_userbadges;';
  $code = implode("\n", $code);

  return array('uuid_features_default_badges' => $code);
}

/**
 * Implements hook_features_revert().
 */
function uuid_badge_features_revert($module) {
  uuid_badge_features_rebuild($module);
}

/**
 * Implements hook_features_rebuild().
 *
 * Rebuilds beans based on UUID from code defaults.
 */
function uuid_badge_features_rebuild($module) {
  $beans = features_get_default('uuid_badge', $module);

  if (!empty($beans)) {
    // Get info about current beans types available in system.
    $entity_info = bean_entity_info();
    // Loop through the export.
    foreach ($beans as $data) {
      // Double-check that bean can be created/reverted.
      if (!isset($entity_info['bean']['bundles'][$data['type']])) {
        drupal_set_message('Bundle not found for Badge of type ' . $data['type'] . '. Badge was not created/reverted.', 'warning');
      }
      else {
        // If this is an update, there will be a by-UUID matching bean.
        $existing = entity_get_id_by_uuid('imagex_userbadge', array($data['uuid']));
        if (!empty($existing)) {
          $imagex_userbadge = entity_load_single('imagex_userbadge', $existing[$data['uuid']]);
          foreach ($data as $key => $value) {
            $imagex_userbadge->$key = $value;
          }
        }
        else {
          // Create a new imagex_userbadge.
          $imagex_userbadge = entity_create('imagex_userbadge', $data);
        }
        // Import file fields.
        uuid_badge_file_field_import($imagex_userbadge, 'imagex_userbadge');
        if (!$imagex_userbadge->save()) {
          drupal_set_message('Failed to create ' . $data['type'] . ' imagex_userbadge ' . $data['label'], 'error');
        }
      }
    }
  }
}

/**
 * Handle exporting file fields.
 */
function uuid_badge_file_field_export(&$export, $entity_type) {
  $export_bundle = 'badge';
  $fields = field_info_instances($entity_type, $export_bundle);
  $supported_fields = array_map('trim', explode(',', variable_get('uuid_features_file_supported_fields', 'file, image')));
  $bundles = variable_get('uuid_features_file_' . $entity_type, array('badge'));
  if ($bundles == '') {
    $bundles = array();
  }
  if (!in_array($export_bundle, $bundles)) {
    foreach ($fields as $field_instance) {
      $field = &$export->{$field_instance['field_name']};
      $info = field_info_field($field_instance['field_name']);
      // If this field should implement file import/export system but
      // filefield exports are not enabled for this entity, just set the field
      // to an empty array.
      if (in_array($info['type'], $supported_fields)) {
        $field = array();
      }
    }
  }
  else {
    $orig_assets_path = $assets_path = variable_get('uuid_features_file_assets_path', '');
    $export_mode = 'inline';

    switch ($export_mode) {
      case 'local':
        $export_var = 'uuid_features_file_path';
        break;
      case 'remote':
        $export_var = 'uuid_features_file_url';
        break;
      default:
      case 'inline':
        $export_var = 'uuid_features_file_data';
        break;
    }
    // If files are supposed to be copied to the assets path.
    if ($export_mode == 'local' && $assets_path) {
      // Ensure the assets path is created
      if ((!is_dir($assets_path) && mkdir($assets_path, 0777, TRUE) == FALSE)
        || !is_writable($assets_path)
      ) {
        // Try creating a public path if the local path isn't writeable.
        // This is a kludgy solution to allow writing file assets to places
        // such as the profiles/myprofile directory, which isn't supposed to
        // be writeable
        $new_assets_path = 'public://' . $assets_path;
        if (!is_dir($new_assets_path) && mkdir($new_assets_path, 0777, TRUE) == FALSE) {
          drupal_set_message(t("Could not create assets path! '!path'", array('!path' => $assets_path)), 'error');
          // Don't continue if the assets path is not ready
          return;
        }
        $assets_path = $new_assets_path;
      }
    }

    // get all fields from this entity
    foreach ($fields as $field_instance) {
      // load field infos to check the type
      $field = &$export->{$field_instance['field_name']};
      $info = field_info_field($field_instance['field_name']);
      // check if this field should implement file import/export system
      if (in_array($info['type'], $supported_fields)) {
        // we need to loop into each language because i18n translation can build
        // fields with different language than the node one.
        foreach($field as $language => $files) {
          if (is_array($files)) {
            foreach($files as $i => $file) {

              // convert file to array to stay into the default uuid_features_file format
              $file = (object) $file;

              // Check the file
              if (!isset($file->uri) || !is_file($file->uri)) {
                drupal_set_message(t("File field found on term, but file doesn't exist on disk? '!path'", array('!path' => $file->uri)), 'error');
                continue;
              }

              if ($export_mode == 'local') {
                if ($assets_path) {
                  // The writeable path may be different from the path that gets saved
                  // during the feature export to handle the public path/local path
                  // dilemma mentioned above.
                  $writeable_export_data = $assets_path . '/' . basename($file->uri);
                  $export_data = $orig_assets_path . '/' . basename($file->uri);
                  if (!copy($file->uri, $writeable_export_data)) {
                    drupal_set_message(t("Export file error, could not copy '%filepath' to '%exportpath'.", array('%filepath' => $file->uri, '%exportpath' => $writeable_export_data)), 'error');
                    return FALSE;
                  }
                }
                else {
                  $export_data = $file->uri;
                }
              }
              // Remote export mode
              elseif ($export_mode == 'remote') {
                $export_data = url($file->uri, array('absolute' => TRUE));
              }
              // Default is 'inline' export mode
              else {
                $export_data = base64_encode(file_get_contents($file->uri));
              }

              // build the field again, and remove fid to be sure that imported node
              // will rebuild the file again, or keep an existing one with a different fid
              $field[$language][$i]['fid'] = NULL;
              $field[$language][$i]['timestamp'] = NULL;
              $field[$language][$i][$export_var] = $export_data;
            }
          }
        }
      }
    }
  }
}

/**
 * Handle importing file fields.
 */
function uuid_badge_file_field_import(&$import, $entity_type) {
  $import_bundle = 'badge';
  // Get all fields from this bundle.
  $fields = field_info_instances($entity_type, $import_bundle);

  foreach($fields as $field_instance) {
    // Load field info to check the type.
    $field = &$import->{$field_instance['field_name']};
    $info = field_info_field($field_instance['field_name']);
    $supported_fields = array_map('trim', explode(',', variable_get('uuid_features_file_supported_fields', 'file, image')));
    // Check if this field should implement file import/export system.
    if (!empty($field) && in_array($info['type'], $supported_fields)) {
      // We need to loop into each language because i18n translation can build
      // fields with different language than the term one.
      foreach($field as $language => $files) {
        if (is_array($files)) {
          foreach($files as $i => $file) {
            // Convert file to array to stay into the default uuid_features_file format.
            $file = (object)$file;
            $result = _uuid_features_file_field_import_file($file);
            // The file was saved successfully, update the file field (by reference).
            if ($result == TRUE && isset($file->fid)) {
              $field[$language][$i] = (array)$file;
            }
            else {
              $field[$language][$i] = array();
            }
          }
        }
      }
    }
  }
}