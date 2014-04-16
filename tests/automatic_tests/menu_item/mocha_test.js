var gui = require('nw.gui');
var assert = require('assert');

describe('MenuItem', function(){
  var menu;
  beforeEach(function(){
    menu = new gui.Menu();
  })

  describe('.type', function(){
    
    it('should be separator', function(){  
      menu.append(new gui.MenuItem({ label : 'Item 1', type : 'separator' }));
      assert.equal(menu.items.length, 1);
      assert.equal(menu.items[0].type, 'separator');
    })

    it('should be normal', function(){
      menu.append(new gui.MenuItem({ label : 'Item 1' }));
      assert.equal(menu.items.length, 1);
      assert.equal(menu.items[0].type, 'normal');
    })

    it('should be checkbox', function(){
      menu.append(new gui.MenuItem({ label : 'Item 1', type : 'checkbox' }));
      assert.equal(menu.items.length, 1);
      assert.equal(menu.items[0].type, 'checkbox');
    })
    
  })

  describe('.label', function(){
    it('set label', function(){
      menu.append(new gui.MenuItem({ label : 'Item 1'}));
      assert.equal(menu.items.length, 1);
      assert.equal(menu.items[0].label, 'Item 1');

    })

    it('after change',function(){
      menu.append(new gui.MenuItem({ label : 'Item 1'}));
      assert.equal(menu.items.length, 1);
      menu.items[0].label = 'Item Mama';
      assert.equal(menu.items[0].label, 'Item Mama');
    })

  })

  describe('.icon', function(){
    it('set menu icon', function(){
      menu.append(new gui.MenuItem({ label : 'Item icon' }));
      assert.equal(menu.items.length, 1);
      menu.items[0].icon = 'icon1.png';
      assert.equal(menu.items[0].icon, 'icon1.png');

    })

    it('clear menu icon', function(){
      menu.append(new gui.MenuItem({ label : 'Item icon' }));
      menu.items[0].icon = 'icon1.png';
      menu.items[0].icon = '';
      assert.equal(menu.items[0].icon, '');
    })
  })

  describe('.tooltip', function(){
    it('set tooltip', function(){
      menu.append(new gui.MenuItem({ label : 'Item 0', tooltip : 'tooltip' }));
      assert.equal(menu.items[0].tooltip, 'tooltip');
    })

    it('change tooltip', function(){
      menu.append(new gui.MenuItem({ label : 'Item 0', tooltip : 'tooltip' }));
      menu.items[0].tooltip = '';
      assert.equal(menu.items[0].tooltip, '');
    })
  })

  
  describe('.checked', function(){
    it('set enabled', function(){
      menu.append(new gui.MenuItem({ type : 'checkbox', label : 'Item 0', checked : true }));
      assert.equal(menu.items[0].checked, true);
    })

    it('change checked', function(){
      menu.append(new gui.MenuItem({ type : 'checkbox', label : 'Item 0', checked: true }));     
      menu.items[0].checked = false;
      assert.equal(menu.items[0].checked, false);

    })
  })
 

  describe('.enabled', function(){
    it('set enabled', function(){
      menu.append(new gui.MenuItem({ type : 'checkbox', label : 'Item 0', enabled : true }));
      assert.equal(menu.items[0].enabled, true);
    })

    it('change enabled', function(){
      menu.append(new gui.MenuItem({ type : 'checkbox', label : 'Item 0', enabled : true }));
      menu.items[0].enabled = false;
      assert.equal(menu.items[0].enabled, false);

    })
  })

  describe('.submenu', function(){
    it('set submenu', function(){
      var submenu = new gui.Menu();
      submenu.append(new gui.MenuItem({ type: 'checkbox', label: 'Sub 1', checked: true, enabled: false }));
      submenu.append(new gui.MenuItem({ type: 'checkbox', label: 'Sub 2', checked: true, enabled: false }));
      submenu.append(new gui.MenuItem({ type: 'checkbox', label: 'Sub 3', checked: true, enabled: false }));
      submenu.append(new gui.MenuItem({ type: 'checkbox', label: 'Sub 4', checked: true, enabled: false }));
      menu.append(new gui.MenuItem({ label: 'I have submenu', submenu: submenu }));

      assert.equal(menu.items.length, 1);
      assert.equal(menu.items[0].label, 'I have submenu');
      assert.equal(menu.items[0].submenu, submenu);
      assert.equal(submenu.items.length, 4);
    })
  })

  describe('#click()', function(){
    
    var menu_item = new gui.MenuItem({label : 'item 1'});
    

    it('before click',function(){
      assert.equal(menu_item.label, 'item 1');    
    })

    it('after click',function(done){
      menu_item.on('click', function(){
        menu_item.label = '2';
        assert.equal(menu_item.label, '2'); 
        done();
      })
      menu_item.emit('click');
         
    })



  })

})
