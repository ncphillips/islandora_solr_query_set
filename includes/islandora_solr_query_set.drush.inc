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
  module_load_include('inc', 'islandora_solr_query_set', 'includes/IslandoraSolrQueryObject');
  $iqs = new IslandoraSolrQueryObject();


}
