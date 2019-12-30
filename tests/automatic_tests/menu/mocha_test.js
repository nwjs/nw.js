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

})


