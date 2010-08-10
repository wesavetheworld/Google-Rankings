<?php
// $Id$
/**
 * @file
 */

/**
 * Settings form.
 */
function google_rankings_settings(&$form_state) {
  $form['google_rankings_toplists_limit'] = array(
    '#type' => 'select',
    '#title' => t('Top list items'),
    '#description' => t('Maximum number of items to show in top lists like top queries, top words and top pages'),
    '#default_value' => variable_get('google_rankings_toplists_limit', GOOGLE_RANKINGS_TOPLISTS_LIMIT_DEFAULT),
    '#options' => array(10 => 10, 25 => 25, 50 => 50, 100 => 100, 250 => 250, 500 => 500, 1000 => 1000),
  );

  $form['google_rankings_light'] = array(
    '#type' => 'checkbox',
    '#title' => t('Light reports'),
    '#description' => t('Less computation heavy reports. If checked the top queries report will miss the target pages column.'),
    '#default_value' => variable_get('google_rankings_light', FALSE),
  );

  return system_settings_form($form);
}
