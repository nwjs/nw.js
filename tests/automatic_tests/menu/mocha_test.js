var gui = require('nw.gui');
var assert = require('assert');


describe('Menu', function(){
  
  describe('#append()', function(){
    it('should append a value', function(){
      var menu = new gui.Menu();
      menu.append(new gui.MenuItem({ label:'Item 1'}));
      assert.equal(menu.items[0].label, 'Item 1');
    })
    
  })

  describe('#.length', function(){
    it('should return correct value', function(){
      var menu = new gui.Menu();
      menu.append(new gui.MenuItem({label : 'Item 1'}));
      menu.append(new gui.MenuItem({label : 'Item 2'}));
      assert.equal(menu.items.length, 2);
    })
  })

  describe('#insert()', function(){
    var menu = new gui.Menu();
    
    it('should adject .length', function(){
      menu.append(new gui.MenuItem({label : 'Item 1'}));
      menu.append(new gui.MenuItem({label : 'Item 2'}));
	  menu.insert(new gui.MenuItem({label : 'Item 0'}), 1);
      assert.equal(menu.items.length, 3);
    })

    it('new value should be added in', function(){
      assert.equal(menu.items[1].label, 'Item 0');
    })

    it('the origin value', function(){
      assert.equal(menu.items[2].label, 'Item 2');
    })
  })

  describe('#removeAt()', function(){
    var menu = new gui.Menu();
    it('should adject .length', function(){
      menu.append(new gui.MenuItem({label : 'Item 0'}));
      menu.append(new gui.MenuItem({label : 'Item 1'}));
      menu.append(new gui.MenuItem({label : 'Item 2'}));
      menu.removeAt(1);
      assert.equal(menu.items.length, 2);
    });

    it('the next value', function(){
      assert.equal(menu.items[1].label, 'Item 2');
    })
  })

  describe('#remove()', function(){
    var menu = new gui.Menu();
    menu.append(new gui.MenuItem({label : 'Item 0'}));
    menu.append(new gui.MenuItem({label : 'Item 1'}));
    var removedItem = new gui.MenuItem({label : 'Item Deleted'});
    menu.insert(removedItem, 1);
    it('before delete', function(){
      assert.equal(menu.items.length, 3);
      assert.equal(menu.items[1].label, 'Item Deleted');
    })

    it('after delete', function(){
      menu.remove(removedItem);
      assert.equal(menu.items.length, 2);
      assert.equal(menu.items[1].label, 'Item 1');

    })
  })

  describe('#popup(), not implentment',function(){
  })

    describe('#.items', function() {
	var menu = new gui.Menu();

	it('should be emtpy', function() {
	    assert.equal(menu.items.length, 0);
	    assert.equal(typeof menu.click, 'undefined');
	});

	
	it('should be one', function() {
	    menu.append(new gui.MenuItem({
		label: 'Item 1',
		click: function() {
		    return 'Click on Item 1';
		}
	    }));

	    assert.equal(menu.items.length, 1);
	    assert.equal(menu.items[0].label, 'Item 1');
	    assert.equal(menu.items[0].icon, '');
	    assert.equal(menu.items[0].tooltip, '');
	    assert.equal(typeof menu.items[0].checked, 'undefined');
	    assert.equal(menu.items[0].enabled, true);
	});
	
	
	it('should be more', function() {

	    menu.append(new gui.MenuItem({ type: 'separator' }));
	    menu.append(new gui.MenuItem({
		label: 'Item 2',
		click: function() {
		    return 'Click on Item 2';
		}
	    }));
	    menu.append(new gui.MenuItem({ label: 'Item 3' }));
	    menu.append(new gui.MenuItem({ label: 'Item 4', tooltip: 'tooltip' }));
	    menu.append(new gui.MenuItem({ label: 'Item 5' }));
	    menu.insert(new gui.MenuItem({ label: 'Item Papa' }), 1);
	    
	    assert.equal(menu.items.length, 7);
	    assert.equal(menu.items[0].label, 'Item 1');
	    assert.equal(menu.items[1].label, 'Item Papa');
	    assert.equal(menu.items[2].type, 'separator');
	    assert.equal(menu.items[3].label, 'Item 2');
	    assert.equal(menu.items[4].label, 'Item 3');
	    assert.equal(menu.items[4].tooltip, '');
	    assert.equal(menu.items[5].label, 'Item 4');
	    assert.equal(menu.items[5].tooltip, 'tooltip');
	    assert.equal(menu.items[6].label, 'Item 5');
	});

    });
    
    describe('menubar', function() {
	var win = gui.Window.get();
	var menubar = new gui.Menu({ type: 'menubar' });
	var sub1 = new gui.Menu();
	sub1.append(new gui.MenuItem({
	    label: 'Test1',
	    click: function() {
		return 'menubar sub1 Test1';
	    }
	}));
	
	sub1.append(new gui.MenuItem({
	    type: 'checkbox',
	    checked: true, //<------------------------------------------------!!!!!!
	    label: 'Test CheckBox'
	}));
	
	var sub2 = new gui.Menu();
	sub2.append(new gui.MenuItem({
	    label: 'Test2',
	    click: function() {
		return 'menubar sub2 Test2';
	    }
	}));
	
	menubar.append(new gui.MenuItem({ label: 'Sub1', submenu: sub1}));
	menubar.append(new gui.MenuItem({ label: 'Sub2', submenu: sub2}));
	it('should set menubar', function() {
	    assert.equal(typeof win.menu, 'undefined')
	    win.menu = menubar;
	    assert.equal(win.menu, menubar);
	});
    })
})


