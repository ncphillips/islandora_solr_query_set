<?php

/*
 *This file should be put in the islandora modules plugins directory
 * typical usage: drush -u 1 islandora_islandora_solr_query_set collection:pid isMemberOf TRUE
 * the above would give you an interactive purge
 * if the last parameter is not TRUE then all the pids in the list will be purged without prompting
 * purging an object cannot be undone so use wisely (you have been warned)
 */

//drush hook

function islandora_solr_query_set_drush_command() {
  $items = array();

  $items['islandora-solr-query-set-test'] = array(
    'description' => "Tests out the Islandora Query Set module's classes.",
    'aliases' => array('iqs_test'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN, // we can pass in users id on the command line using drush -u.
  );

  return $items;
}

//drush hook
function drush_islandora_solr_query_set_test() {
  module_load_include('inc', 'islandora_solr_query', 'includes/IslandoraSolrQuerySet');
  $iqs = new IslandoraSolrQuerySet();

  /**
   * dctitle is "Islandora Bioinformatics" or "BANANA"
   * AND
   * dcname is not "John Dow"
   */
  $dc_title= array(
    '#not' => array('a'=> ['b'],),
    '#or' => array('a'=> array('b')),
  );


//  $query = $iqs->build_field_subquery('', $dc_title);
//  drush_print('FIELD QUERY: ' . $query);


  /**
   * CModel is specimen but not (microbe or tunicate)
   */
  $cmodels = array(
    'islandora:specimen_cmodel',
    '#not' => array(
      'islandora:microbe_cmodel',
      '#or' => array(
        'islandora:tunicate_cmodel',
      )
    )
  );








//  $query = $iqs->build_cmodel_subquery($cmodels);
//  drush_print('CMODEL QUERY: ' . $query);

}

