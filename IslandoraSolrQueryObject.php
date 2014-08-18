<?php
/**
 * Created by PhpStorm.
 * User: nolan
 * Date: 2014-08-14
 * Time: 4:51 PM
 */

namespace all\modules\islandora_solr_query_set;


class IslandoraSolrQueryObject {

  protected $id = '';
  protected $models = '';
  protected $model = '';

  protected $qs = null;

  public function __construct($solr_object, $url='http://localhost', $port='8080'){
    $qs = new IslandoraSolrQuerySet();

    // Set the models.
    // Set the parent model.
  }

  public function getIslandoraObject(){
    return islandora_object_load($this->id);
  }
} 