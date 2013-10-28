<?php

/**
 * @file
 * Contains the basic 'badge' field handler.
 */

/**
 * Field handler to provide simple renderer that allows linking to a badge.
 * Definition terms:
 * - link_to_badge default: Should this field have the checkbox "link to badge" enabled by default.
 *
 * @ingroup views_field_handlers
 */
class imagex_userbadge_handler_field_badge extends views_handler_field {

  function init(&$view, &$options) {
    parent::init($view, $options);
    // Don't add the additional fields to groupby
    if (!empty($this->options['link_to_badge'])) {
      $this->additional_fields['badge_id'] = array('table' => 'imagex_userbadge', 'field' => 'badge_id');
      if (module_exists('translation')) {
        $this->additional_fields['language'] = array('table' => 'imagex_userbadge', 'field' => 'language');
      }
    }
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['link_to_badge'] = array('default' => isset($this->definition['link_to_badge default']) ? $this->definition['link_to_badge default'] : FALSE, 'bool' => TRUE);
    return $options;
  }

  /**
   * Provide link to node option
   */
  function options_form(&$form, &$form_state) {
    $form['link_to_badge'] = array(
      '#title' => t('Link this field to the original badge'),
      '#description' => t("Enable to override this field's links."),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['link_to_badge']),
    );

    parent::options_form($form, $form_state);
  }

  /**
   * Render whatever the data is as a link to the node.
   *
   * Data should be made XSS safe prior to calling this function.
   */
  function render_link($data, $values) {
    if (!empty($this->options['link_to_badge']) && !empty($this->additional_fields['badge_id'])) {
      if ($data !== NULL && $data !== '') {
        $this->options['alter']['make_link'] = TRUE;
        $this->options['alter']['path'] = "badge/" . $this->get_value($values, 'badge_id');
        if (isset($this->aliases['language'])) {
          $languages = language_list();
          $language = $this->get_value($values, 'language');
          if (isset($languages[$language])) {
            $this->options['alter']['language'] = $languages[$language];
          }
          else {
            unset($this->options['alter']['language']);
          }
        }
      }
      else {
        $this->options['alter']['make_link'] = FALSE;
      }
    }
    return $data;
  }

  function render($values) {
    $value = $this->get_value($values);
    return $this->render_link($this->sanitize_value($value), $values);
  }
}
