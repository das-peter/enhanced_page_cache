
-- SUMMARY --

This module is a demo how to make the drupal internal page cache more flexible.
To achive that you have to change some CORE CODE. The changes are described 
below in -- CORE CHANGES --. 
A appropriate patch is shipped with the module.


-- REQUIREMENTS --

None.


-- INSTALLATION --

* Install as usual, see http://drupal.org/node/70151 for further information.

* Apply core patch shipped with the module or manually change the core code 
  according to the documentation of the changes in -- CORE CHANGES --

* Enable page caching: admin/config/development/performance
  - Enable "Cache pages for anonymous users"
  - Configure the default cache lifetimes. 



-- CORE CHANGES --

## drupal_page_get_cache() in includes/bootstrap.inc:

  Original Code:
  --------------
  
  $cache = cache_get($base_root . request_uri(), 'cache_page');
  
  
  New Code:
  ---------
  
  $cid = $base_root . request_uri();
  // Allow modules to alter the cid.
  drupal_alter('page_cache_cid', $cid);
  $cache = cache_get($cid, 'cache_page');
  
  
## drupal_page_set_cache() in includes/common.inc:

  Original Code:
  ----------------
  
    $cache = (object) array(
      'cid' => $base_root . request_uri(),
      'data' => array(
        'path' => $_GET['q'],
        'body' => ob_get_clean(),
        'title' => drupal_get_title(),
        'headers' => array(),
      ),
      'expire' => CACHE_TEMPORARY,
      'created' => REQUEST_TIME,
    );
    
    
  New Code:
  ---------
  
  $body = ob_get_clean();
    $cache = (object) array(
      'cid' => $base_root . request_uri(),
      'data' => array(
        'path' => $_GET['q'],
        'body' => $body,
        'title' => drupal_get_title(),
        'headers' => array(),
      ),
      'expire' => CACHE_TEMPORARY,
      'created' => REQUEST_TIME,
    );

    // Allow modules to alter the cache object.
    drupal_alter('page_cache_object', $cache);
    // If caching got disabled - fill the output buffer again and return.
    if (!$cache) {
      ob_start();
      echo $body;
      return FALSE;
    }
    
    
-- FAQ --

Q: Is it possible to cache the pages language dependent.
A: Yes it is, but you've to enable the language path prefix or the language 
   subomain. This is the only way since the language negotiation happens later 
   in the bootstrap - thus you can't add the language to the page cache cid.