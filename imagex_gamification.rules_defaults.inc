<?php
/**
 * @file
 * imagex_gamification.rules_defaults.inc
 */

/**
 * Implements hook_default_rules_configuration().
 */
function imagex_gamification_default_rules_configuration() {
  $items = array();
  if (module_exists('comment')){
    $items['rules_imagex_userpoints_comment'] = entity_import('rules_config', '{ "rules_imagex_userpoints_comment" : {
        "LABEL" : "Userpoints Comment",
        "PLUGIN" : "reaction rule",
        "TAGS" : [ "Comments", "Userpoints" ],
        "REQUIRES" : [ "userpoints_rules", "comment" ],
        "ON" : [ "comment_insert" ],
        "DO" : [
          { "userpoints_action_grant_points" : {
              "user" : [ "comment:author" ],
              "points" : "1",
              "tid" : "0",
              "entity" : [ "" ],
              "operation" : "Add a userpoint for commenting",
              "display" : 0,
              "moderate" : "approved"
            }
          }
        ]
      }
    }');
  }
  if (module_exists('forum')) {
    $items['rules_imagex_userpoints_forum'] = entity_import('rules_config', '{ "rules_imagex_userpoints_forum" : {
        "LABEL" : "Userpoints Forum",
        "PLUGIN" : "reaction rule",
        "TAGS" : [ "Forum", "Userpoints" ],
        "REQUIRES" : [ "rules", "userpoints_rules" ],
        "ON" : [ "node_insert" ],
        "IF" : [
          { "node_is_of_type" : { "node" : [ "node" ], "type" : { "value" : { "forum" : "forum" } } } }
        ],
        "DO" : [
          { "userpoints_action_grant_points" : {
              "user" : [ "node:author" ],
              "points" : "1",
              "tid" : "0",
              "entity" : [ "" ],
              "operation" : "Add a userpoint for creating a forum topic",
              "display" : 0,
              "moderate" : "default"
            }
          }
        ]
      }
    }');
  }
  if (module_exists('imagex_answers')) {
    $items['rules_imagex_userpoints_question'] = entity_import('rules_config', '{ "rules_imagex_userpoints_question" : {
        "LABEL" : "Userpoints Question \\u0026 Answer",
        "PLUGIN" : "reaction rule",
        "TAGS" : [ "Q\\u0026A", "Userpoints" ],
        "REQUIRES" : [ "rules", "userpoints_rules" ],
        "ON" : [ "node_insert" ],
        "IF" : [
          { "node_is_of_type" : {
              "node" : [ "node" ],
              "type" : { "value" : { "answer" : "answer", "question" : "question" } }
            }
          }
        ],
        "DO" : [
          { "userpoints_action_grant_points" : {
              "user" : [ "node:author" ],
              "points" : "1",
              "tid" : "0",
              "entity" : [ "" ],
              "operation" : "Add a userpoint for Question or Answer",
              "display" : 1,
              "moderate" : "default"
            }
          }
        ]
      }
    }');
  }
  return $items;
}
