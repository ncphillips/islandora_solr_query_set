<?php
/**
 * Created by PhpStorm.
 * User: nolan
 * Date: 2014-08-14
 * Time: 4:40 PM
 */

@include_once 'sites/all/libraries/tuque/MagicProperty.php';

class IslandoraSolrQuerySet extends MagicProperty implements \ArrayAccess, \Iterator{
  // Query Grammar Arrays
  public $fields;
  public $cmodels;

  // The array of objects in the
  private $objects;
  private $query;
  private $query_processor;
  private $page;
  private $page_size;

  public function __construct($url='http://localhost', $port='8080'){
    $this->cmodels = array();
    $this->fields = array();
    $this->objects = array();
    $this->query = '';
    $this->page = 0;
    $this->page_size = variable_get('islandora_solr_num_of_results', 20);
    $this->query_processor = new IslandoraSolrQueryProcessor();
    islandora_xacml_api_islandora_solr_query($this->query_processor);
  }


  private function execute_query(){
    $this->build_query();

    $this->sq->buildAndExecuteQuery($this->query);
  }

  // Building queries from grammar arrays
  public function build_query(){
    /** @todo */
    $cmq = $this->build_cmodel_subquery($this->cmodels);


    return "($cmq) AND {$fq}";
  }

  private function build_cmodel_subquery($cmq_array){
    $cmq_array_keys = array_keys($cmq_array) ? array_keys($cmq_array) : array();

    $value_qstring = '';
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
      $values[] = 'RELS_EXT_hasModel_uri_ms:"' . $value .'"';
//      $value_strings[] = 'RELS_EXT_hasModel_uri_ms:"info:fedora/' . $value .'"';
    }

    $value_qstring = implode(' AND ', $values);
    if (sizeof($values) > 1) {
      //  If there are many values, wrap them in braces.
      $value_qstring = "($value_qstring)";
    }
    elseif (!$value_qstring){
      // If there are no values, set as $field:*
      $value_qstring = 'RELS_EXT_hasModel_uri_ms:*';
    }

    // Not (Values)
    if ($not_string){
      $value_qstring = "($value_qstring NOT $not_string)";
    }

    // Or (Values)
    if ($or_string){
      $value_qstring = "($value_qstring OR $or_string)";
    }

    return $value_qstring;
  }

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
        $values[] = "$field:\"$value\"";
      }
    }
    else{
      // Default catch all. Might be a cleverer way around this.
      $values[] = '*:*';
    }

    // Values
    $value_string = implode(' AND ', $values);
    if (sizeof($values) > 1) {
      $value_string = "($value_string)";
    }
    elseif (!$value_string and $field){
      $value_string = "$field:*";
    }

    // Not (Values)
    if ($not_string){
      $value_string = "($value_string NOT $not_string)";
    }

    // Or (Values)
    if ($or_string and $not_string and $values){
      $value_string = "($value_string OR $or_string)";
    }

    // Field values
    if ($f_string){
      $value_string = "($value_string AND $f_string)";
    }

    return $value_string;

  }




  /** Functions for ArrayAccess Interface */
  public function offsetExists($index){ /** @todo */}
  public function offsetGet($index){ /** @todo */}
  public function offsetSet($index, $value){ /** Not implemented */}
  public function offsetUnset($index){ /** Not implemented */}

  /** Functions for Iterator interface */
  public function current(){ }
  public function key(){}
  public function next(){}
  public function rewind(){}
  public function valid(){}

  /** Function for IteratorAggregate interface */
  public function getIterator(){
    return new IslandoraSolrQuerySetIterator();
  }

}