<?php
// $Id$
/**
 * @file
 *
 * Variables available:
 * - $domains: Google domains.
 * - $top_queries: List of top queries.
 * - $top_words: List of top words.
 * - $top_pages: List of top pages.
 */
?>

<fieldset>
  <a name="domains"></a>
  <h2><?php print t('Domains') ?></h2>
  <?php print $domains ?>
</fieldset>

<div class="top-queries">
  <fieldset>
    <a name="top_queries"></a>
    <h2><?php print t('Top Queries') ?></h2>
    <?php print $top_queries ?>
  </fieldset>
</div>

<div class="top-words">
  <fieldset>
    <a name="top_words"></a>
    <h2><?php print t('Top Words') ?></h2>
    <?php print $top_words ?>
  </fieldset>
</div>

<div class="top-pages">
  <fieldset>
    <a name="top_pages"></a>
    <h2><?php print t('Top Pages') ?></h2>
    <?php print $top_pages ?>
  </fieldset>
</div>

