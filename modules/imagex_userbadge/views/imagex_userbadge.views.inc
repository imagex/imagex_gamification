<?php

/**
 * @file
 * Providing extra functionality for the badge UI via views.
 */


/**
 * Implements hook_views_data()
 */
function imagex_userbadge_views_data_alter(&$data) { 
  $data['imagex_userbadge']['link_imagex_userbadge'] = array(
    'field' => array(
      'title' => t('Link'),
      'help' => t('Provide a link to the badge.'),
      'handler' => 'imagex_userbadge_handler_link_field',
    ),
  );
  $data['imagex_userbadge']['edit_imagex_userbadge'] = array(
    'field' => array(
      'title' => t('Edit Link'),
      'help' => t('Provide a link to the edit form for the badge.'),
      'handler' => 'imagex_userbadge_handler_edit_link_field',
    ),
  );
  $data['imagex_userbadge']['delete_imagex_userbadge'] = array(
    'field' => array(
      'title' => t('Delete Link'),
      'help' => t('Provide a link to delete the badge.'),
      'handler' => 'imagex_userbadge_handler_delete_link_field',
    ),
  );
  $data['imagex_userbadge']['operations'] = array(
    'field' => array(
      'title' => t('Operations links'),
      'help' => t('Display all operations available for this badge.'),
      'handler' => 'imagex_userbadge_handler_badge_operations_field',
    ),
  );
  // OVERRIDES
  // Add our handler to the badge_id and name fields
  $data['imagex_userbadge']['badge_id']['field'] = array(
    'handler' => 'imagex_userbadge_handler_field_badge',
    'click sortable' => TRUE,
  );
  $data['imagex_userbadge']['name']['field'] = array(
    'field' => 'name', // the real field. This could be left out since it is the same.
    'handler' => 'imagex_userbadge_handler_field_badge',
    'click sortable' => TRUE,
    'link_to_badge default' => TRUE,
   );
  
  // Make created & updated field a date
  $data['imagex_userbadge']['created'] = array(
    'title' => t('Creation date'), // The item it appears as on the UI,
    'help' => t('The date the badge was created.'), // The help that appears on the UI,
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  // changed field
  $data['imagex_userbadge']['changed'] = array(
    'title' => t('Updated date'), // The item it appears as on the UI,
    'help' => t('The date the badge was last updated.'), // The help that appears on the UI,
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );
}


/**
 * Implements hook_views_default_views().
 */
function imagex_userbadge_views_default_views() {
  $views = array();
  dpm(__FUNCTION__);
  // Default admin page for Badges
  $view = new view();
  $view->name = 'imagex_userbadge';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'imagex_userbadge';
  $view->human_name = 'Badges';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  
  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Badges';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'administer user badges';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'badge_id' => 'badge_id',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'badge_id' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['label'] = 'Empty ';
  $handler->display->display_options['empty']['area']['empty'] = FALSE;
  $handler->display->display_options['empty']['area']['content'] = 'No badges have been created yet';
  /* Field: User Badge: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'imagex_userbadge';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['label'] = 'Badge Name';
  $handler->display->display_options['fields']['name']['element_label_colon'] = FALSE;
  /* Field: User Badge: Updated date */
  $handler->display->display_options['fields']['changed']['id'] = 'changed';
  $handler->display->display_options['fields']['changed']['table'] = 'imagex_userbadge';
  $handler->display->display_options['fields']['changed']['field'] = 'changed';
  $handler->display->display_options['fields']['changed']['label'] = 'Created/Updated';
  $handler->display->display_options['fields']['changed']['date_format'] = 'time ago';
  /* Field: User Badge: Operations links */
  $handler->display->display_options['fields']['operations']['id'] = 'operations';
  $handler->display->display_options['fields']['operations']['table'] = 'imagex_userbadge';
  $handler->display->display_options['fields']['operations']['field'] = 'operations';
  
  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->display->display_options['path'] = 'admin/people/badges';
  $handler->display->display_options['menu']['type'] = 'tab';
  $handler->display->display_options['menu']['title'] = 'Badges';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $handler->display->display_options['tab_options']['title'] = 'Badges';
  $handler->display->display_options['tab_options']['weight'] = '0';

  $views[$view->name] = $view;
  return $views;
}
