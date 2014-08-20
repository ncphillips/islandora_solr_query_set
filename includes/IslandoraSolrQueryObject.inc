<?php
/**
 * Created by PhpStorm.
 * User: nolan
 * Date: 2014-08-14
 * Time: 4:51 PM
 */


class IslandoraSolrQueryObject extends \MagicProperty{

  private $__fields;
  protected $id;
  protected $label;
  protected $queryset;
  private $islandora_object;
  private $solr_object;

  public function __construct($id_or_solr_array){
    if(is_array($id_or_solr_array)){
      $solr_object = $id_or_solr_array;
    }
    else{
      $this->id = $id_or_solr_array;
      $this->islandora_object = islandora_object_load($id_or_solr_array);
      $this->queryset = new IslandoraSolrQueryset();

      $qp = new IslandoraSolrQueryProcessor();
      $qp->buildAndExecuteQuery("PID:\"{$id_or_solr_array}\"");
      $solr_document = $qp->islandoraSolrResult;

      // There should only be one object.
      $solr_objects = $solr_document['response']['objects'];

      if(sizeof($solr_objects != 1)){
        /** @todo Throw Error */
      }

      $solr_object = $solr_objects[0];
    }
    $this->id = $solr_object['PID'];
    $this->label = $solr_object['object_label'];
    $this->solr_doc = $solr_object['solr_doc'];
  }

  protected function relationshipsMagicProperty($action, $value){
    switch($action){
      case 'get':
        foreach($this->solr_doc as $field => $v){

        }
        break;
    }
  }

  private function is_relationship_field($field, $value){
    $rels_ext_string = '/^RELS_EXT_/';
    $match = preg_match($rels_ext_string, $field);
    if($match){
      $ns_match = '';
    }
  }

  public function __toString(){
    return (String)$this->id;
  }

} 