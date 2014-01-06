<?php

/**
 * @file
 * Contains a Views field handler to take care of displaying deletes links
 * as fields
 */


class imagex_userbadge_handler_delete_link_field extends imagex_userbadge_handler_link_field {
  function construct() {
    parent::construct();
  }


  function render($values) {
    
    //Creating a dummy badge to check access against
    $dummy_badge = (object) array('type' => 'badge');
    if (!imagex_userbadge_access('edit', $dummy_badge)) {
      return;
    }
    
    $text = !empty($this->options['text']) ? $this->options['text'] : t('delete');
    $badge_id = $values->{$this->aliases['badge_id']};
    
    return l($text, 'badge/' . $badge_id . '/delete');
  }
}
