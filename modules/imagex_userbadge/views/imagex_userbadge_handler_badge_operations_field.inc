<?php

/**
 * This field handler aggregates operations that can be done on a badge
 * under a single field providing a more flexible way to present them in a view
 */
class imagex_userbadge_handler_badge_operations_field extends views_handler_field {
  function construct() {
    parent::construct();

    $this->additional_fields['badge_id'] = 'badge_id';
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {

    $links = menu_contextual_links('imagex_userbadge', 'badge', array($this->get_value($values, 'badge_id')));
    
    if (!empty($links)) {
      return theme('links', array('links' => $links, 'attributes' => array('class' => array('links', 'inline', 'operations'))));
    }
  }
}
