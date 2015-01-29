var gui = require('nw.gui');
var assert = require('assert');

describe('Tray', function(){

  var ICON_PATH = './automatic_tests/tray/icon.png';
  var ICON_PATH_BIS = './automatic_tests/tray/another-icon.png';
  var tray;

  afterEach(function(){
    tray.remove();
  });

  describe('.icon', function(){
    before(function() { 
      tray = new gui.Tray({ icon: ICON_PATH });
    });

    it('should create a tray item with the given icon', function(){
      assert.equal(tray.icon, ICON_PATH);
    });

    it('should change icon', function(){
      tray.icon = ICON_PATH_BIS;
      assert.equal(tray.icon, ICON_PATH_BIS);
    });
  });

  describe('.alticon', function(){
    before(function() { 
      tray = new gui.Tray({ icon: ICON_PATH, alticon: ICON_PATH_BIS });
    });

    it('should create a tray item with the given icon', function(){
      assert.equal(tray.alticon, ICON_PATH_BIS);
    });

    it('should change alticon', function(){
      tray.alticon = ICON_PATH;
      assert.equal(tray.alticon, ICON_PATH);
    });
  });

  describe('.iconsAreTemplates', function(){
    before(function() { 
      tray = new gui.Tray({ icon: ICON_PATH, iconsAreTemplates: true });
    });

    it('should create a tray item with icons as templates', function(){
      assert.equal(tray.iconsAreTemplates, true);
    });

    it('should change icons as templates', function(){
      tray.iconsAreTemplates = false;
      assert.equal(tray.iconsAreTemplates, false);
    });
  });

  describe('.title', function(){
    before(function() {
      tray = new gui.Tray({ title: 'nwjs' });
    });

    it('should create a tray item with the given title', function() {
      assert.equal(tray.title, 'nwjs');
    });

    it('should change title', function(){
      tray.title = 'another-title';
      assert.equal(tray.title, 'another-title');
    });
  });

  describe('.tooltip', function(){
    before(function() {
      tray = new gui.Tray({ icon: ICON_PATH, tooltip: 'a tooltip' });
    });

    it('should create a tray item with the given tooltip', function() {
      assert.equal(tray.tooltip, 'a tooltip');
    });

    it('should change tooltip', function(){
      tray.tooltip = 'another tooltip';
      assert.equal(tray.tooltip, 'another tooltip');
    });
  });

  describe('.menu', function() {
    it('should create a tray item with the given menu', function() {
      var trayMenu = new gui.Menu();
      trayMenu.append(new gui.MenuItem({ label: 'an apple a day' }));
      tray = new gui.Tray({ icon: ICON_PATH, menu: trayMenu });
      assert.equal(tray.menu, trayMenu);
    });

    it('should change menu', function() {
      var trayMenu = new gui.Menu();
      trayMenu.append(new gui.MenuItem({ label: 'takes the doctor away' }));
      tray.menu = trayMenu;
      assert.equal(trayMenu, trayMenu);
    });

  });

  describe('.click', function(){
    it('should create a tray item with the given click callback', function() {
      var clicked = function clicked() { done(); };
      var tray = new gui.Tray({ icon: ICON_PATH, click: clicked });
      tray.emit('click');
    });
  });

});
