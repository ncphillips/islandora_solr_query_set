<?php
/**
 * Created by PhpStorm.
 * User: nolan
 * Date: 2014-08-14
 * Time: 4:40 PM
 */

@include_once 'sites/all/libraries/tuque/MagicProperty.php';

class IslandoraSolrQueryset extends MagicProperty{
  // Query Grammar Arrays
  public $fields;
  public $cmodels;

  // The array of objects in the
  private $__objects;
  private $__query;
  private $__page;
  private $query_processor;

  public function __construct($url='http://localhost', $port='8080'){
    $this->cmodels = array();
    $this->fields = array();
    $this->__objects = array();
    $this->__query = '';
    $this->__page = 0;
    $this->query_processor = new IslandoraSolrQueryProcessor();
    islandora_xacml_api_islandora_solr_query($this->query_processor);
  }


  protected function pageMagicProperty($action, $value){
    switch ($action) {
      case 'get':
        return $this->__page;
        break;
      case 'set':
        $this->__page = $value;
        $this->objectsMagicProperty('get', '');
        break;
    }
  }

  protected function queryMagicProperty($action, $value) {
    switch($action){
      case 'get':
        return $this->build_query();
        break;
      case 'isset':
        return isset($this->__query);
        break;
    }
  }

  /** Number of objects found by query. */
  protected function num_foundMagicProperty($action, $value){
    switch($action){
      case 'get':
        return $this->query_processor->islandoraSolrResult['response']['numFound'];
        break;
      case 'isset':
        return isset($this->query_processor->islandoraSolrResult);
        break;

    }
  }

  /** Number of pages found by query. */
  protected function num_pagesMagicProperty($action, $value){
    switch($action){
      case 'get':
        return (int)($this->num_found/$this->page_size) + 1;
        break;
      case 'isset':
        return $this->num_foundMagicProperty('isset', '');
        break;
    }
  }

  /** Number of objects on each page. */
  protected function page_sizeMagicProperty($action, $value){
    switch($action){
      case 'get':
        return variable_get('islandora_solr_num_of_results', 20);
        break;
      case 'isset':
        return true;
        break;
    }
  }

  protected function objectsMagicProperty($actions, $value){
    switch($actions){
      case 'get':
        if (empty($this->__objects)){
          $this->build_and_execute_query();
        }
        return $this->__objects;
        break;
    }
  }

  /**
   * build_and_execute_query
   *
   * @see IslandoraSolrQueryset::build_query
   * @see IslandoraSolrQueryset::execute_query
   */
  private function build_and_execute_query(){
    $this->build_query();
    $this->execute_query();
  }

  /**
   * Sets the $_GET['page'] variable and then executes a solr query
   * using SolrQueryProcessor and then empties the $__objects property before to
   * prepare for the new results.
   *
   * @see IslandoraSolrQueryProcessor::buildAndExecuteQuery
   */
  private function execute_query(){
    $_GET['page'] = $this->page;
    $this->query_processor->buildAndExecuteQuery($this->__query);
    $this->__objects = array();

    $solr_results = $this->query_processor->islandoraSolrResult;
    foreach($solr_results['response']['objects'] as $object){
      $this->__objects[] = new IslandoraSolrQueryObject($object);
    }
  }

  /**
   * Runs the subquery builders and concats them together.
   * @return string
   * The final Solr query.
   *
   * @see IslandoraSolrQueryset::build_cmodel_subquery
   * @see IslandoraSolrQueryset::build_field_subquery
   */
  private function build_query(){
    /** @todo */
    $cmq = $this->build_cmodel_subquery($this->cmodels);
    $fq = $this->build_field_subquery('', $this->fields);

    $q_a = array();
    if($cmq)
      $q_a[] = $cmq;

    if($fq)
      $q_a[] = $fq;

    $this->__query = implode(' AND ', $q_a);

    return $this->__query;
  }

  /**
   * @param $cmq_array
   * @return string
   *
   * See README for explanation of cmodel array grammar.
   */
  private function build_cmodel_subquery($cmq_array){
    $cmq_array_keys = array_keys($cmq_array) ? array_keys($cmq_array) : array();

    $value_string = '';
    $or_string = '';
    $not_string = '';

    // Evaluate #or statements.
    $has_or = array_intersect(array('#or'), $cmq_array_keys);
    if ($has_or){
      $or_string = $this->build_cmodel_subquery($cmq_array['#or']);
      unset($cmq_array['#or']);
    }

    // Evaluate #not statements.
    $has_not = array_intersect(array('#not'), $cmq_array_keys);
    if ($has_not){
      $not_string = $this->build_cmodel_subquery($cmq_array['#not']);
      unset($cmq_array['#not']);
    }

    // Evaluate the Values.
    $values = array();
    foreach ($cmq_array as $value){
      $values[] = 'RELS_EXT_hasModel_uri_ms:' . str_replace(':', '\:', $value);
//      $value_strings[] = 'RELS_EXT_hasModel_uri_ms:"info:fedora/' . $value .'"';
    }

    $value_string = implode(' AND ', $values);
    if (sizeof($values) > 1) {
      $value_string = "($value_string)";
    }

    $query = $value_string;

    // Not (Values)
    if ($not_string){
      if($query){
        $query = "($query NOT $not_string)";
      }
      else{
        $query = "(*:* NOT $not_string)";
      }
    }

    // Or (Values)
    if ($query and $or_string){
      $value_string = "($value_string OR $or_string)";
    }

    return $value_string;
  }

  /**
   * @param $field
   * @param $fa
   * @return string
   *
   * See the README for an explanation of field array grammar.
   */
  private function build_field_subquery($field, $fa){
    $fa_keys = array_keys($fa) ? array_keys($fa) : array();

    $value_string = '';
    $or_string = '';
    $not_string = '';

    // Evaluate #or statements first.
    $has_or = array_intersect(array('#or'), $fa_keys);
    if ($has_or){
      $or_string = $this->build_field_subquery($field, $fa['#or']);
      unset($fa['#or']);
    }

    // Next, evaluate #not statements.
    $has_not = array_intersect(array('#not'), $fa_keys);
    if ($has_not){
      $not_string = $this->build_field_subquery($field, $fa['#not']);
      unset($fa['#not']);
    }

    // Evaluate other fields
    $f_strings = array();
    $fa_keys = array_keys($fa);
    foreach ($fa_keys as $f){
      if(is_string($f)){
        $f_strings[] = $this->build_field_subquery($f, $fa[$f]);
        unset($fa[$f]);
      }
    }
    $f_string = implode(' AND ', $f_strings);

    // Finally, evaluate the values.
    $values = array();
    if ($field){
      foreach ($fa as $value){
        $values[] = "$field:" . str_replace(':', '\:', $value);
      }
    }

    // Values
    $value_string = implode(' AND ', $values);
    if (sizeof($values) > 1) {
      $value_string = "($value_string)";
    }
    elseif (!$value_string and $field){
      $value_string = '';
    }

    $query = $value_string;

    // Field values
    if ($f_string){
      if ($query){
        $query = "($query AND $f_string)";
      }
      else {
        $query = $f_string;
      }
    }

    // Not (Values)
    if ($not_string){
      if($query){
        $query = "($query NOT $not_string)";
      }
      else {
        $query = "(*:* NOT $not_string)";
      }
    }

    // Or (Values)
    if ($query and $or_string){
      $query = "($query OR $or_string)";
    }

    return $query;

  }

}