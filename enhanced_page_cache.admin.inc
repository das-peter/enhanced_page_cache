<?php

// TODO BUILD OUR OWN ADMIN INTERFACE
// - Create Configuration to include / exclude pages from cache.
// - Create Configuration to configure lifetime by path pattern.
// - Rules integration to enhance the above mentioned functionalities?

function enhanced_page_cache_configuration_form($form, &$form_state) {
  $page_cacheability = variable_get('enhanced_page_cache_cacheability', PAGE_CACHEABILITY_NOTLISTED);
  $page_cacheability_pages = variable_get('enhanced_page_cache_cacheability_pages');


  // Cacheability settings.
  $form['cacheability'] = array(
    '#type' => 'item',
    '#title' => t('Enhanced page cache settings'),
  );

  $period = drupal_map_assoc(array(0, 60, 180, 300, 600, 900, 1800, 2700, 3600, 10800, 21600, 32400, 43200, 86400), 'format_interval');
  $period[-1] = '<' . t('Default') . '>';
  $period[-2] = '<' . t('Tomorrow') . '>';
  $period[-3] = '<' . t('Next week') . '>';
  $form['cacheability']['enhanced_page_cache_expiration'] = array(
    '#type' => 'select',
    '#title' => t('Page cache expiration'),
    '#default_value' => variable_get('enhanced_page_cache_expiration', CACHE_TEMPORARY),
    '#options' => $period,
    '#description' => t('Define when this pages cache has to be refreshed.')
  );

  // Per-path cacheability.
  $form['cacheability']['path'] = array(
    '#type' => 'fieldset',
    '#title' => t('Page cacheability'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#group' => 'cacheability',
    '#weight' => 0,
  );

  $access = user_access('use PHP for settings');
  if ($page_cacheability == PAGE_CACHEABILITY_PHP && !$access) {
    $form['cacheability']['path']['cacheability'] = array(
      '#type' => 'value',
      '#value' => PAGE_CACHEABILITY_PHP,
    );
    $form['cacheability']['path']['pages'] = array(
      '#type' => 'value',
      '#value' => $page_cacheability_pages,
    );
  }
  else {
    $options = array(
      PAGE_CACHEABILITY_NOTLISTED => t('All pages except those listed'),
      PAGE_CACHEABILITY_LISTED => t('Only the listed pages'),
    );
    $description = t("Specify pages by using their paths. Enter one path per line. The '*' character is a wildcard. Example paths are %blog for the blog page and %blog-wildcard for every personal blog. %front is the front page.", array('%blog' => 'blog', '%blog-wildcard' => 'blog/*', '%front' => '<front>'));

    if (module_exists('php') && $access) {
      $options += array(PAGE_CACHEABILITY_PHP => t('Pages on which this PHP code returns <code>TRUE</code> (experts only)'));
      $title = t('Pages or PHP code');
      $description .= ' ' . t('If the PHP option is chosen, enter PHP code between %php. Note that executing incorrect PHP code can break your Drupal site.', array('%php' => '<?php ?>'));
    }
    else {
      $title = t('Pages');
    }
    $form['cacheability']['path']['enhanced_page_cache_cacheability'] = array(
      '#type' => 'radios',
      '#title' => t('Cache specific pages'),
      '#options' => $options,
      '#default_value' => $page_cacheability,
    );
    $form['cacheability']['path']['enhanced_page_cache_cacheability_pages'] = array(
      '#type' => 'textarea',
      '#title' => '<span class="element-invisible">' . $title . '</span>',
      '#default_value' => $page_cacheability_pages,
      '#description' => $description,
    );
  }

  return system_settings_form($form);
}