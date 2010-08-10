<?php
// $Id$
/**
 * @file
 */

/**
 * Main overview page callback.
 */
function google_rankings_overview_page($year = NULL, $month = NULL, $day = NULL) {
  drupal_add_css(drupal_get_path('module', 'google_rankings') .'/google-rankings.css');

  if (db_result(db_query("SELECT MIN(rid) FROM {google_rankings}")) == 0) {
    drupal_set_title(t('Google rankings'));
    return t('There are no statistics available. Please check the !status_report page or wait for the next cron run.', array('!status_report' => l('status report', 'admin/reports/status')));
  }

  if ($year == NULL || $month == NULL) {
    list($year, $month) = explode(' ', date('Y m'));
  }

  // Month overview
  if ($day == NULL) {
    drupal_set_title(t('Google rankings') .' - '. format_date(strtotime($year .'-'. $month), 'custom', 'Y. F'));

    $months   = google_rankings_overview_months();
    $days     = google_rankings_overview_days($year, $month);

    $domains      = google_rankings_overview_domains($year, $month);
    $top_queries  = google_rankings_overview_top_queries($year, $month);
    $top_words    = google_rankings_overview_top_words($year, $month);
    $top_pages    = google_rankings_overview_top_pages($year, $month);

    return theme('google_rankings_month_overview', $months, $days, $domains, $top_queries, $top_words, $top_pages);
  }
  // Day overview
  else {
    drupal_set_title(t('Google rankings') .' - '. format_date(strtotime($year .'-'. $month .'-'. $day), 'custom', 'Y. F j.'));

    $domains      = google_rankings_overview_domains($year, $month, $day);
    $top_queries  = google_rankings_overview_top_queries($year, $month, $day);
    $top_words    = google_rankings_overview_top_words($year, $month, $day);
    $top_pages    = google_rankings_overview_top_pages($year, $month, $day);

    return theme('google_rankings_day_overview', $months, $days, $domains, $top_queries, $top_words, $top_pages);
  }
}

/**
 * Monthly ranking reports.
 */
function google_rankings_overview_months() {
  $header = array(t('Month'), t('PI'), t('Different queries'), t('Different pages'), t('Average Position'), t('Average Page'));

  $bounds  = db_fetch_object(db_query("SELECT MIN(timestamp) AS first, MAX(timestamp) AS last FROM {google_rankings}"));
  list ($first_year, $first_month)  = explode(' ', date('Y n', $bounds->first)); // ex.: "2009 11"
  list ($last_year, $last_month)    = explode(' ', date('Y n', $bounds->last)); // ex.: "2010 6"

  for ($y = $first_year; $y <= $last_year; $y++) {
    for ($m = 1; $m <= 12; $m++) {
      if ($y == $first_year && $m < $first_month) continue;
      if ($y == $last_year  && $m > $last_month)  continue;

      $begin  = strtotime(sprintf('%d-%02d-01 00:00:00', $y, $m));
      $end    = strtotime("+1month -1day", $begin);

      $link = l(format_date($begin, 'custom', 'Y. F'), 'admin/reports/google-rankings/overview/'. sprintf('%d/%02d', $y, $m));
      $data = _google_rankings_overview_row($begin, $end);
      $rows[] = array_merge(array($link), $data);
    }
  }

  return theme('table', $header, $rows);
}

/**
 * Daily ranking reports.
 */
function google_rankings_overview_days($year, $month) {
  $header = array(t('Day'), t('PI'), t('Different queries'), t('Different pages'), t('Average Position'), t('Average Page'));

  $last_day = date('t', strtotime($year .'-'. $month .'-01'));

  for ($d = 1; $d <= $last_day; $d++) {
    $begin  = strtotime(sprintf('%d-%02d-%02d 00:00:00', $year, $month, $d));
    $end    = $begin + 86399;

    $data = _google_rankings_overview_row($begin, $end);
    $day  = format_date($begin, 'custom', 'Y. M j.');
    $link = $data[0] > 0 ? l($day, 'admin/reports/google-rankings/overview/'. sprintf('%d/%02d/%02d', $year, $month, $d)) : $day;
    $rows[] = array_merge(array($link), $data);
  }

  return theme('table', $header, $rows);
}

/**
 * Helper function, returns one row of data.
 */
function _google_rankings_overview_row($begin, $end) {
  $o = db_fetch_object(db_query("
    SELECT COUNT(*) as hits, COUNT(DISTINCT query) AS queries, COUNT(DISTINCT path) paths, AVG(position) avg_position, AVG(page) avg_page
    FROM {google_rankings}
    WHERE %d <= timestamp AND timestamp <= %s",
    $begin,
    $end
  ));

  return array(
    number_format($o->hits, 0, '.', ' '),
    number_format($o->queries, 0, '.', ' '),
    number_format($o->paths, 0, '.', ' '),
    number_format($o->avg_position, 2, '.', ' '),
    number_format($o->avg_page, 2, '.', ' '),
  );
}

/**
 * Helper function to calculate period boundaries in unix timestamp.
 */
function _google_rankings_boundaries($year = NULL, $month = NULL, $day = NULL) {
  if ($year == NULL || $month == NULL) {
    $begin  = 0;
    $end    = 2147483647; // 19 Jan 2038 03:14:07 UTC
  }
  elseif ($day == NULL) {
    $begin  = strtotime(sprintf('%d-%02d-01 00:00:00', $year, $month));
    $end    = strtotime('+1 month', $begin) - 1;
  }
  else {
    $begin  = strtotime(sprintf('%d-%02d-%02d 00:00:00', $year, $month, $day));
    $end    = $begin + 86399;
  }
  return array($begin, $end);
}

/**
 * Referring Google domains.
 */
function google_rankings_overview_domains($year = NULL, $month = NULL, $day = NULL) {
  list($begin, $end) = _google_rankings_boundaries($year, $month, $day);

  $sum = db_result(db_query('SELECT COUNT(*) FROM {google_rankings} WHERE %d <= timestamp AND timestamp <= %d', $begin, $end));

  $domains = array();
  $result = db_query("
    SELECT google_domain, COUNT(*) AS cnt
    FROM {google_rankings}
    WHERE %d <= timestamp AND timestamp <= %d
    GROUP BY google_domain
    ORDER BY COUNT(*) DESC",
    $begin,
    $end
  );
  while ($domain = db_fetch_object($result)) {
    $percent = (int) round($domain->cnt / $sum * 100);
    $domains[] = sprintf('<strong>%s</strong>:&nbsp;<span title="%s">%s%%</span>', check_plain($domain->google_domain), number_format($domain->cnt), ($percent == 0 ? '>1' : $percent));
  }

  return implode(', ', $domains);
}

/**
 * Top queries table.
 */
function google_rankings_overview_top_queries($year = NULL, $month = NULL, $day = NULL) {
  list($begin, $end) = _google_rankings_boundaries($year, $month, $day);

  $limit = variable_get('google_rankings_toplists_limit', GOOGLE_RANKINGS_TOPLISTS_LIMIT_DEFAULT);

  $header = array(t('Query'), t('PI'), t('Pos.'));
  if (!variable_get('google_rankings_light', FALSE)) $header[] = t('Pages');

  $results = db_query("
    SELECT query, COUNT(*) count, AVG(position) avg_posistion
    FROM {google_rankings}
    WHERE %d <= timestamp AND timestamp <= %d
    GROUP BY query
    ORDER BY COUNT(*) DESC
    LIMIT 0, %d",
    $begin,
    $end,
    $limit
  );

  while ($o = db_fetch_object($results)) {
    $row = array(
      check_plain($o->query),
      number_format($o->count, 0, ',', ' '),
      $o->avg_posistion == NULL ? 'n/a' : number_format($o->avg_posistion, 2, ',', ' ')
    );

    // Get paths
    if (!variable_get('google_rankings_light', FALSE)) {
      $paths_results = db_query("
        SELECT path, COUNT(*) count
        FROM {google_rankings}
        WHERE %d <= timestamp AND timestamp <= %d
          AND query = '%s'
        GROUP BY path
        ORDER BY COUNT(*) DESC",
        $begin,
        $end,
        $o->query
      );

      $paths = array();
      while ($p = db_fetch_object($paths_results)) {
        $paths[$p->path] = $p->count;
      }

      $path_list = array();
      foreach ($paths as $path => $hits) {
        $path_list[] = l($path, $path) . sprintf(' (%d%%)', $hits/array_sum($paths) * 100);
        if (count($path_list) == 2) break;
      }
      $row[] = implode('<br />', $path_list);
    }

    $rows[] = $row;
  }

  return theme('table', $header, $rows);
}

/**
 * Top words table.
 */
function google_rankings_overview_top_words($year, $month, $day = NULL) {
  list($begin, $end) = _google_rankings_boundaries($year, $month, $day);

  $limit = variable_get('google_rankings_toplists_limit', GOOGLE_RANKINGS_TOPLISTS_LIMIT_DEFAULT);

  $header = array(t('Word'), t('PI'));

  $results = db_query("
    SELECT query, COUNT(*) count
    FROM {google_rankings}
    WHERE %d <= timestamp AND timestamp <= %d
    GROUP BY query",
    $begin,
    $end
  );

  $words = array();

  while ($o = db_fetch_object($results)) {
    preg_match_all('|[\S]+|', check_plain($o->query), $m);
    foreach ($m[0] as $word) $words[$word] += $o->count;
  }
  arsort($words);

  foreach ($words as $word => $hits) {
    $rows[] = array(
      $word,
      number_format($hits, 0, '', ' ')
    );
    if (count($rows) == $limit) break;
  }

  return theme('table', $header, $rows);
}

/**
 * Top pages table.
 */
function google_rankings_overview_top_pages($year, $month, $day = NULL) {
  list($begin, $end) = _google_rankings_boundaries($year, $month, $day);

  $limit = variable_get('google_rankings_toplists_limit', GOOGLE_RANKINGS_TOPLISTS_LIMIT_DEFAULT);

  $header = array(t('Page'), t('PI'));

  $results = db_query("
    SELECT path, COUNT(*) count
    FROM {google_rankings}
    WHERE %d <= timestamp AND timestamp <= %d
    GROUP BY path
    ORDER BY COUNT(*) DESC
    LIMIT 0, %d",
    $begin,
    $end,
    $limit
  );

  while ($o = db_fetch_object($results)) {
    $rows[] = array(
      l($o->path, $o->path),
      number_format($o->count, 0, ',', ' '),
    );
  }

  return theme('table', $header, $rows);
}

/**
 * Individual clicks report page callback.
 */
function google_rankings_clicks_page() {
  drupal_add_css(drupal_get_path('module', 'google_rankings') .'/google-rankings.css');

  if (db_result(db_query("SELECT MIN(rid) FROM {google_rankings}")) == 0) {
    drupal_set_title(t('Google rankings'));
    return t('There are no statistics available. Please check the !status_report page or wait for the next cron run.', array('!status_report' => l('status report', 'admin/reports/status')));
  }

  $header = array(
    array('data' => t('Timestamp'), 'field' => 'r.timestamp', 'sort' => 'desc'),
    array('data' => t('Domain'), 'field' => 'r.google_hostname'),
    array('data' => t('Query'), 'field' => 'r.query'),
    array('data' => t('Page'), 'field' => 'r.path'),
    array('data' => t('User'), 'field' => 'u.name'),
    array('data' => t('SERP Position'), 'field' => 'r.position'),
    array('data' => t('SERP Page'), 'field' => 'r.page'),
  );

  $sql = "SELECT r.* FROM {google_rankings} r LEFT JOIN {users} u ON u.uid = r.uid";
  $sql_cnt = "SELECT COUNT(*) FROM {google_rankings}";

  $result = pager_query($sql . tablesort_sql($header), 30);
  $rows = array();
  while ($log = db_fetch_object($result)) {
    $rows[] = array(
      array('data' => format_date($log->timestamp, 'small'), 'class' => 'nowrap'),
      array('data' => $log->google_hostname),
      array('data' => $log->query),
      _statistics_format_item($log->title, $log->path),
      theme('username', $log),
      array('data' => ($log->position ? $log->position .'.' : ''), 'class' => 'google-ranking'),
      array('data' => ($log->page ? $log->page .'.' : ''), 'class' => 'google-ranking'),
      );
  }

  return theme('table', $header, $rows) . theme('pager', NULL, 30, 0);
}

