# Islandora Solr Queryset

## Example Arrays

### Basic Logic

a

    array('a')

not a

    array('#not' => array('a'))

a and b

    array('a', 'b')

a or b

    array(
      'a',
      '#or' => array('b')
    )

a not b

    array(
      'a',
      '#not' => array('b')
    )

a and b not c

    array(
      'a', 'b',
      '#not' => array('c'),
    )

not (a or b)

    array(
      '#not' => array(
        'a',
        '#or' => array('b')
      )
    )

### CModel Examples

This grammar follows the basic logic, with the field always assumed to be
the CModel field.

Specimen

    array(
      'islandora:specimen_cmodel',
    )

Specimen, not Microbes

    array(
      'islandora:specimen_cmodel',
      '#not' => 'islandora:microbe_cmodel',
    )

### Field Examples
Title is "Islandora Bioinformatics":

    array(
      'title' => array(
        'Islandora Bioinformatics',
      )


Title is "Islandora Bioinformatics", author is not Nolan Phillips:

    array(
      'title' => array(
        'Islandora Bioinformatics',
      ),
      'author' => array(
        '#not' => array('Nolan Phillips'),
      ),
    )

(Title is "Islandora Bioinformatics") and (author is not Nolan Phillips, or
it was published before 2014).

    array(
      'title' => array(
        'Islandora Bioinformatics',
      ),
      'author' => array(
        '#not' => array('Nolan Phillips'),
        '#or' => array(
          'publishing_year' => array('[* to 2014]'),
         )
      )
    )

### Relationship Examples
Relationship arrays will be used in the IslandoraQuerysetResult object to
generate queries for finding it's relative objects. This grammar does not follow
the field's or fa

All objects related to this one.

    array( )

All objects with a fedora-system:isMemberOf or fedora-system:isMetadataFor
relationship to the current object.

    array(
      array(
        'alias' => 'fedora-system',
        'url' => 'info:fedora/fedora-system:def/relations-external#',
        'relationships' => array(
          'isMemberOf',
          'isMetadataFor',
        ),
      )
    )


## Appendix A: Array Grammars
These are the grammar's describing the arrays. They have not been placed in
Chompsky Normal Form, because I believe doing reduces some redundancy and makes
 things easier to read.

#### Exponents

 In these definitinos you will see symbols raised to some exponent, this is the
 maximum number of times this symbol can appear in the array, with the minimum
 always being 1. For example, [a^3] means an array with 1-3 a's in it.

#### n
 Let __n__ be some positive integer.

### CModel Array

__C__:

* [ ]
* D
* D merge [ '#or' => M ]

__D__:

* [ v^n ]
* [ '#not' => D ]
* [ v^n, '#not' => D ]

__v__:

* A fedora pid for a CModel object.

### Solr Fields Array

__F__:

* [ ]
* G
* G merge [ '#or' => F ]

__G__:

* H
* [ '#not' => H ]
* H merge [ '#not' => H ]

__H__:

* [ v^n ]
* [ v^n, f^k]

__v__:

* A value for any Solr field

__f__:

* 'field_name' => F
    * A key-value pair where the key is the field name, and the value an __F__

### Solr Query Array

__Q__:

* [ ]
* R
* R merge ['#or' => Q]

__R__:

* [ f^n ]
* [ '#not' => Q ]
* [ f^n, '#not' => Q]

__f__:

* 'field_name' => F
    * A key-value pair where the key is the field name, and the value an __F__
    * See the Solr Fields Array grammar.

