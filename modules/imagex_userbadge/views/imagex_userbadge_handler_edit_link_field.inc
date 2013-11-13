<?php

/**
 * @file
 * Contains a Views field handler to take care of displaying edit links
 * as fields
 */


class imagex_userbadge_handler_edit_link_field extends imagex_userbadge_handler_link_field {
  function construct() {
    parent::construct();
  }


  function render($values) {
    //Creating a dummy badge to check access against
    $dummy_badge = (object) array('type' => 'badge');
    if (!imagex_userbadge_access('edit', $dummy_badge)) {
      return;
    }
    
    $text = !empty($this->options['text']) ? $this->options['text'] : t('edit');
    $badge_id = $values->{$this->aliases['badge_id']};
    
    return l($text, 'admin/people/badges/badge/' . $badge_id . '/edit');
  }
}
