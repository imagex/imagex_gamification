<?php
/**
 * User Badge Template
 */
?>
<div id="user-badge-<?php print $imagex_userbadge->badge_id;?>"  class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>
  
  <?php print render($title_prefix); ?>
  <?php if (!$page): ?>
    <h2<?php print $title_attributes; ?>><a href="<?php print $url; ?>"><?php print $title; ?></a></h2>
  <?php endif; ?>
  <?php print render($title_suffix); ?>
  
  <div class="content"<?php print $content_attributes; ?>>
    <?php print render($content);?>
  </div>
</div>
