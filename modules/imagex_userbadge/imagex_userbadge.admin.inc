<?php

/**
 * @file
 * badge editing UI.
 *
 * We make very little use of the EntityAPI interface for this - preferring instead to use
 * views. That offers more flexibility to change a UI that will, more often than not,
 * be end-user facing.
 */

/**
 * UI controller.
 */
class ImagexUserBadgeUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults. Main reason for doing this is that
   * parent class hook_menu() is optimized for entity type administration.
   */
  public function hook_menu() {
    $items = array();
    $id_count = count(explode('/', $this->path));
    $wildcard = isset($this->entityInfo['admin ui']['menu wildcard']) ? $this->entityInfo['admin ui']['menu wildcard'] : '%' . $this->entityType;

    $items[$this->path] = array(
      'title' => 'Badges',
      'description' => 'Add, edit, and update badges.',
    );
    
    // Change the overview menu type for the list of badges.
    $items[$this->path]['type'] = MENU_LOCAL_TASK;
    
    // Change the add page menu to multiple types of entities
    $items[$this->path . '/add'] = array(
      'title' => 'Add Badge',
      'page callback' => 'imagex_userbadge_form_wrapper',
      'page arguments' => array(imagex_userbadge_create(array('type' => 'badge'))),
      'access callback' => 'imagex_userbadge_access',
      'file' => 'imagex_userbadge.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module'])
    );
    
    // Loading and editing badge entities
    $items[$this->path . '/badge/' . $wildcard] = array(
      'page callback' => 'imagex_userbadge_form_wrapper',
      'page arguments' => array($id_count + 1),
      'access callback' => 'imagex_userbadge_access',
      'weight' => 0,
      'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
      'file' => 'imagex_userbadge.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module'])
    );
    $items['badge/' . $wildcard . '/edit'] = array(
      'title' => 'Edit',
      'type' => MENU_LOCAL_TASK,
      'weight' => -5,
      'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
      'page callback' => 'imagex_userbadge_form_wrapper',
      'page arguments' => array(1),
      'access callback' => 'imagex_userbadge_access',
      'weight' => 0,
      'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
      'file' => 'imagex_userbadge.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module'])
    );
    
    $items['badge/' . $wildcard . '/delete'] = array(
      'title' => 'Delete',
      'page callback' => 'imagex_userbadge_delete_form_wrapper',
      'page arguments' => array(1),
      'access callback' => 'imagex_userbadge_access',
      'type' => MENU_LOCAL_TASK,
      'context' => MENU_CONTEXT_INLINE,
      'weight' => 10,
      'file' => 'imagex_userbadge.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module'])
    );
    
    // Menu item for viewing badges
    $items['badge/' . $wildcard] = array(
      //'title' => 'Title',
      'title callback' => 'imagex_userbadge_page_title',
      'title arguments' => array(1),
      'page callback' => 'imagex_userbadge_page_view',
      'page arguments' => array(1),
      'access callback' => 'imagex_userbadge_access',
      'access arguments' => array('view', 1),
      'type' => MENU_CALLBACK,
    );
    
    $items['badge/' . $wildcard . '/view'] = array(
      'title' => 'View',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,
    );
    return $items;
  }
  
  
  /**
   * Create the markup for the add badge Entities page within the class
   * so it can easily be extended/overriden.
   */ 
  public function addPage() {
    $item = menu_get_item();
    $content = system_admin_menu_block($item);

    if (count($content) == 1) {
      $item = array_shift($content);
      drupal_goto($item['href']);
    }    
        
    return array('#markup' => 'Blah Blah Blah');
  }
  
}


/**
 * Form callback wrapper: create or edit a badge.
 *
 * @param $badge
 *   The badge object being edited by this form.
 *
 * @see imagex_userbadge_edit_form()
 */
function imagex_userbadge_form_wrapper($badge) {
  // Add the breadcrumb for the form's location.
  imagex_userbadge_set_breadcrumb();
  return drupal_get_form('imagex_userbadge_edit_form', $badge);
}


/**
 * Form callback wrapper: delete a badge.
 *
 * @param $badge
 *   The badge object being edited by this form.
 *
 * @see imagex_userbadge_edit_form()
 */
function imagex_userbadge_delete_form_wrapper($badge) {
  // Add the breadcrumb for the form's location.
  //imagex_userbadge_set_breadcrumb();
  return drupal_get_form('imagex_userbadge_delete_form', $badge);
}


/**
 * Form callback: create or edit a badge.
 *
 * @param $badge
 *   The badge object to edit or for a create form an empty badge object
 *     with only a badge type defined.
 */
function imagex_userbadge_edit_form($form, &$form_state, $badge) {
  // Add the default field elements.
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Badge Name'),
    '#default_value' => isset($badge->name) ? $badge->name : '',
    '#maxlength' => 255,
    '#required' => TRUE,
    '#weight' => -5,
  );

  // Add the field related form elements.
  $form_state['imagex_userbadge'] = $badge;

  field_attach_form('imagex_userbadge', $badge, $form, $form_state);

  $form['actions'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-actions')),
    '#weight' => 400,
  );

  // We add the form's #submit array to this button along with the actual submit
  // handler to preserve any submit handlers added by a form callback_wrapper.
  $submit = array();

  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save badge'),
    '#submit' => $submit + array('imagex_userbadge_edit_form_submit'),
  );
  
  if (!empty($badge->name)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete badge'),
      '#suffix' => l(t('Cancel'), 'admin/people/badges'),
      '#submit' => $submit + array('imagex_userbadge_form_submit_delete'),
      '#weight' => 45,
    );
  }

  // We append the validate handler to #validate in case a form callback_wrapper
  // is used to add validate handlers earlier.
  $form['#validate'][] = 'imagex_userbadge_edit_form_validate';
  return $form;
}


/**
 * Form API validate callback for the badge form
 */
function imagex_userbadge_edit_form_validate(&$form, &$form_state) {
  $badge = $form_state['imagex_userbadge'];
  
  // Notify field widgets to validate their data.
  field_attach_form_validate('imagex_userbadge', $badge, $form, $form_state);
}


/**
 * Form API submit callback for the badge form.
 * 
 * @todo remove hard-coded link
 */
function imagex_userbadge_edit_form_submit(&$form, &$form_state) {
  
  $badge = entity_ui_controller('imagex_userbadge')->entityFormSubmitBuildEntity($form, $form_state);
  // Save the badge and go back to the list of badges
  
  // Add in created and changed times.
  if ($badge->is_new = isset($badge->is_new) ? $badge->is_new : 0){
    $badge->created = time();
  }

  $badge->changed = time();
  
  $badge->save();
  $form_state['redirect'] = 'badge/' . $badge->badge_id;
}

/**
 * Form API submit callback for the delete button.
 * 
 * @todo Remove hard-coded path
 */
function imagex_userbadge_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/people/badges/badge/' . $form_state['imagex_userbadge']->badge_id . '/delete';
}


/**
 * Form callback: confirmation form for deleting a badge.
 *
 * @param $badge
 *   The badge to delete
 *
 * @see confirm_form()
 */
function imagex_userbadge_delete_form($form, &$form_state, $badge) {
  $form_state['imagex_userbadge'] = $badge;

  $form['#submit'][] = 'imagex_userbadge_delete_form_submit';

  $form = confirm_form($form,
    t('Are you sure you want to delete badge %name?', array('%name' => $badge->name)),
    'admin/people/badges/badge',
    '<p>' . t('This action cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );
  
  return $form;
}

/**
 * Submit callback for badge_delete_form
 */
function imagex_userbadge_delete_form_submit($form, &$form_state) {
  $badge = $form_state['imagex_userbadge'];

  imagex_userbadge_delete($badge);

  drupal_set_message(t('The badge %name has been deleted.', array('%name' => $badge->name)));
  watchdog('imagex_userbadge', 'Deleted badge %name.', array('%name' => $badge->name));

  $form_state['redirect'] = 'admin/people/badges';
}



/**
 * Page to add badge Entities.
 *
 * @todo Pass this through a proper theme function
 */
function imagex_userbadge_add_page() {
  $controller = entity_ui_controller('imagex_userbadge');
  return $controller->addPage();
}

/**
 * Sets the breadcrumb for administrative badge pages.
 */
function imagex_userbadge_set_breadcrumb() {
  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Administration'), 'admin'),
    l(t('People'), 'admin/people'),
    l(t('Badges'), 'admin/people/badges'),
  );

  drupal_set_breadcrumb($breadcrumb);
}

/**
 * Settings form for User Badges
 */
function imagex_userbadge_admin_page($form, &$form_state) {
  
}