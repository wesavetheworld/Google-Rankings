<?php
// $Id$
/**
 * @file
 *
 * Variables available:
 * - $months: Table of months.
 * - $days: Table of days.
 * - $domains: Google domains.
 * - $top_queries: List of top queries.
 * - $top_words: List of top words.
 * - $top_pages: List of top pages.
 */
?>
<fieldset>
  <legend><?php print t('Statistics') ?></legend>
  <a href="#months"><?php print t('Months') ?></a> |
  <a href="#days"><?php print t('Days') ?></a> |
  <a href="#domains"><?php print t('Domains') ?></a> |
  <a href="#top_queries"><?php print t('Top Queries') ?></a> |
  <a href="#top_words"><?php print t('Top Words') ?></a> |
  <a href="#top_pages"><?php print t('Top Pages') ?></a>
</fieldset>

<fieldset>
  <a name="months"></a>
  <h2><?php print t('Months') ?></h2>
  <?php print $months ?>
</fieldset>

<fieldset>
  <a name="days"></a>
  <h2><?php print t('Days') ?></h2>
  <?php print $days ?>
</fieldset>

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

