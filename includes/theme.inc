<?php

function theme_bootstrap() {
  common_attach_css(array(
    'boot' => '/css/bootstrap.min.css',
    'main' => '/css/main.css',
  ));
  theme_page();
}

function theme_page() {
  theme_header();
  if (isset($_GET['p'])) {
    $page = preg_replace('/[^\da-z]/i', '_', $_GET['p']);
    $func = 'theme_page_' . $page;
    if (!function_exists($func)) {
      theme_page_404();
      return;
    }
    $func();    
  }
  else {
    theme_page_index();
  }
}


function theme_build_nav_link($nav, $text) {
  $link = '<li class="' . (isset($_GET['p']) && $_GET['p'] == $nav['query']['p'] && !isset($nav['query']['theme']) ? 'active' : '')  .'">';
  $url = $nav['base'];
  reset($nav['query']);
  $first = key($nav['query']);
  foreach ($nav['query'] as $query => $data) {
    $url .= ($query === $first ? '?' : '&') . $query . '=' . $data;
  }
  $link .= '<a href="' . $url  . '">' . $text  . '</a>';
  $link .= '</li>';
  return $link;
}

function theme_header() {
  global $header;
  global $navigation;
  $header .= '
    <nav class="navbar navbar-inverse">
    <div class="contianer-fluid">
    <div class="navbar-header">
      <a class="navbar-brand" href="#">BaDDeV</a>
    </div> 
    <div class="collapse navbar-collapse" id="nav">
      <ul class="nav navbar-nav">';
      foreach ($navigation as $text => $nav) {
        $header .= theme_build_nav_link($nav, $text);
      }
    $header .= '</ul>';
    if (isset($_SESSION['uid'])) {
      $query = mysql_database_query("SELECT mail FROM users WHERE uid='{$_SESSION['uid']}'");
      $result = mysql_database_get_results($query);
      $header .= '<p class="navbar-text navbar-right">Logged in as ' . $result[0]['mail']  . '&nbsp;&nbsp;</p>';
    }
    $header .= '</div>
    </div>
    </nav>
  ';
}

function theme_page_i_dont_understand() {
  global $body;
  $body = 'hi mer';
}

function theme_page_index() {
  global $body;
  $body .= '<h2>Welcome to BaDDeV</h2>
            <p>BaDDev is a sandboxed website designed to teach developers about common exploits; how they work and how to fix them.</p>
            <p>Developers should never place this website on a server OR network you care about.  It is extremly dangerous and <b>very</b> vulnerable.</p>
            <p><b>NOTE:</b> Exploitation of this server is allowed under the condition that the users do not deface files (Attacking profiles and dynamic pages in the form of XSS <b>is</b> allowed).</p>
            <p>Also, please do not use real email address or real passwords on this site.  Consider ever single piece of data on this site to be logged by either the site or an attacker.</p>
            <p>Test out the highlight functionality <a href="/?p=index&highlight=baddev">here</a></p>
';
}

function theme_page_404() {
  global $body;
  $body = '<h1>404</h1>
           <p><b>' . $_GET['p'] . '</b> not found!</p>';
}

function theme_page_login() {
  global $body;
  if (isset($_POST['mail'], $_POST['password'])) {
    $sql = "SELECT uid FROM users WHERE mail='{$_POST['mail']}' AND password=MD5('{$_POST['password']}');";
    $body .= $sql;
    $query = mysql_database_query($sql);
    $result = mysql_database_get_results($query);
    if (empty($result)) {
      $body .= '<div class="alert alert-danger" role="alert">
                 <b>Incorrect username or password</b> Please check your input and try again.
                </div>';
   }
   else {
     $body .= 'logged in';
     common_login_user($result[0]['uid']);
     header('Location: /');
   }
  }
  $body .= '<form action="?p=login" method="POST" class="form-signin">
            <h2 class="form-signin-heading">Please sign in</h2>           
            <input type="text" name="mail" class="form-control" placeholder="Username" autocomplete="off">
            <br>
            <input type="password" name="password" class="form-control" placeholder="Password">
            <br>
            <input type="submit" class="btn btn-success btn-block">
            </form>';
}

function theme_page_register() {
  global $body;
  if (isset($_POST['mail'])
  || isset($_POST['password'])
  || isset($_POST['bio'])) {
    if (empty($_POST['mail'])
    ||  empty($_POST['password'])
    ||  empty($_POST['bio'])) {
      $body .= '<div class="alert alert-danger" role="alert"><b>Please fill out the whole form!</b></div>';
    }
    else {
      if (!filter_var($_POST['mail'], FILTER_VALIDATE_EMAIL)) {
        $body .= '<div class="alert alert-danger" role="alert">Not a valid email address</div>';
      }
      else {
        $pass = md5($_POST['password']);
        $sql = "INSERT INTO users (mail, password, bio, created) VALUES ('{$_POST['mail']}', '{$pass}', '{$_POST['bio']}', unix_timestamp(now()))";
        $query = mysql_database_query($sql);
        $body .= '<div class="alert alert-success" role="alert">' . $_POST['mail']  . ' is now registered!</div>';
      }
    }
  }
  $body .= '<form action="?p=register" method="POST" class="form-register" id="register">
            <h2 class="form-register-heading">Register New User</h2>
            <input type="text" name="mail" class="form-control" placeholder="Email"><br>
            <input type="password" name="password" class="form-control" placeholder="Password"><br>
            <textarea class="form-control" rows="4" cols="50" name="bio" form="register" placeholder="Bio..."></textarea><br>
            <input type="submit" class="btn btn-success btn-block">
            </form>';
}

function theme_page_list() {
  global $body;
  $query = mysql_database_query("SELECT uid, mail, bio FROM users;");
  $results = mysql_database_get_results($query);
  $body .= '<table class="table">';
  $body .= '<thead><tr>
              <th>User Mail</th>
              <th>User Bio</th>
            </tr></thead><tbody>';
  foreach ($results as $result) {
    $body .= '<tr class="uid-' . $result['uid'] . '">
                <td><a href="/?p=user&u=' . $result['uid']  . '">' . $result['mail']  . '</a></td>
                <td>' . htmlentities($result['bio'])  . '</td>
              </tr>';
  }
  $body .= '</tbody></table>';
}

function theme_page_logout() {
  global $body;
  session_destroy();
  $body .= "You are now logged out.";
}

function theme_page_user() {
  global $body;
  if (!isset($_GET['u'])) {
    $body .= 'No user selected';
    return;
  }
  $uid = $_GET['u'];
  $sql = "SELECT uid, mail, bio, created FROM users WHERE uid='{$uid}';";
  print $sql;
  $query = mysql_database_query($sql);
  $result = mysql_database_get_results($query);
  if (empty($result)) {
    $body .= 'User ' . $uid  . ' does not exist';
    return;
  }
  $user = current($result);
  $body .= "<h2>{$user['mail']}</h2>";
  $body .= '<span>Joined ' . date("m/d/Y", $user['created'])  . '</span>';
  $body .= '<p>' . $user['bio']  . '</p>';
}

function theme_page_cal() {
  global $body;
  $body .= '<a href="/?p=cal&arg=2060">View the year 2060</a><br>';
  $arguments = '';
  if (isset($_GET['arg'])) {
    $arguments = ' ' . $_GET['arg'];
  }
  exec('cal' . $arguments, $cal);
  foreach ($cal as $line) {
    $body .= $line . '<br>';
  }
}

function theme_page_user_edit() {
  global $body;
  global $connection;
  if (!isset($_SESSION['uid'])) {
    $body .= 'Please log in to edit user form';
    return;
  }
  if (isset($_POST['bio']) && !empty($_POST['bio'])) {
    mysql_database_query('UPDATE users SET bio="' . $connection->escape_string($_POST['bio']) . '" WHERE uid="' . $_SESSION['uid']  . '"');
    $body .= '<div class="alert alert-success" role="alert">User bio updated</div>';
  }

  if (isset($_POST['password']) && !empty($_POST['password'])) {
    $password = md5($_POST['password']);
    mysql_database_query('UPDATE users SET password="' . $password . '" WHERE uid="' . $_SESSION['uid']  . '"');
    $body .= '<div class="alert alert-success" role="alert">User password updated</div>';
  }
 
  $sql = "SELECT * FROM users WHERE uid={$_SESSION['uid']}";
  $query = mysql_database_query($sql);
  $result = mysql_database_get_results($query);
  $user = current($result);
  $body .= '<form action="?p=user-edit" method="POST" class="form-user-edit" id="user-edit">
            <h2 class="form-user-edit-heading">Edit User ' . $user['mail'] . '</h2>
            <input type="password" name="password" class="form-control" placeholder="Password"><br>
            <textarea class="form-control" rows="4" cols="50" name="bio" form="user-edit" placeholder="Bio...">' . $user['bio']  . '</textarea><br>
            <input type="submit" class="btn btn-success btn-block">
            </form>';
}

function theme_alter() {
  global $body;
  if (isset($_GET['highlight'])) {
    $body = preg_replace('/' . $_GET['highlight'] . '/i', '<b class="highlight">' . $_GET['highlight']  . '</b>', $body);
  }

  if (isset($_GET['theme'])) {
    $filename = str_replace("../", '', '/var/www/html/theme/' . $_GET['theme']);
    $file = fopen($filename, 'r');
    $body .= '<style>' . fread($file, filesize($filename)) . '</style>';    
  }
  if (isset($_GET['include'])) {
    include_once($_GET['include'] . '.php');
  }
}

function theme_page_search() {
  global $body;
  if (isset($_GET['search'])) {
    $body .= '<div class="alert alert-danger" role="alert">
                Search term <b>' . $_GET['search'] . '</b> not found.
              </div>';
  }
  $body .= '<form action="?p=search" method="GET" class="form-search">
            <h2 class="form-signin-heading">Search Term</h2>
            <input type="hidden" name="p" value="search">
            <input type="text" name="search" class="form-control" placeholder="Search..">
            <input type="submit" class="btn btn-success btn-block" value="Search">
            </form><br><small><i><b>Note:</b> this form doesn\'t actually search so don\'t bother with SQL Injection</i></small>';
}
