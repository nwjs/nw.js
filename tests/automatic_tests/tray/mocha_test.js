var gui = require('nw.gui');
var assert = require('assert');

describe('Tray', function() {
    var menu = new gui.Menu();

    menu.append(new gui.MenuItem({ type: 'checkbox', label: 'Sub 1', checked: true, enabled: false }));
    menu.append(new gui.MenuItem({ type: 'checkbox', label: 'Sub 2', checked: false, enabled: true }));
    menu.append(new gui.MenuItem({ type: 'checkbox', label: 'Sub 3', checked: false, enabled: true }));
    menu.append(new gui.MenuItem({ type: 'checkbox', label: 'Sub 4', checked: false, enabled: true }));
    
    // Click to flip other items
    var lastone = menu.items[0];
    menu.items[0].click = menu.items[1].click = menu.items[2].click = menu.items[3].click = function() {
	lastone.checked = false;
	lastone.enabled = true;
	this.enabled = false;
	lastone = this;
    };
    
    
    var tray = new gui.Tray({
	title: 'Test Tray',
	icon: './automatic_tests/tray/icon.png',
	menu: menu
    });
    
    describe('#.icon', function() {
    	it('should return correct icon', function() {
    	    assert.equal(tray.icon, './automatic_tests/tray/icon.png');
    	    tray.icon = '';
    	    assert.equal(tray.icon, '');
    	    tray.icon = './automatic_tests/tray/icon.png';
    	    assert.equal(tray.icon, './automatic_tests/tray/icon.png');
    	});
    });

    describe('#.title', function() {
    	it('should return correct title', function() {
    	    assert.equal(tray.title, 'Test Tray');
    	    tray.title = '';
    	    assert.equal(tray.title, '');
    	    tray.title = 'Test Tray';
    	    assert.equal(tray.title, 'Test Tray');
    	});
    });
    
    describe('#.tooltip', function() {
    	it('should return correct tooltip', function() {
    	    assert.equal(tray.tooltip, '');
    	    tray.tooltip = 'Love Tooltip';
    	    assert.equal(tray.tooltip, 'Love Tooltip');
    	    tray.tooltip = '';
    	    assert.equal(tray.tooltip, '');
    	});
    });

    describe('#.menu', function() {
    	it('should return correct menu', function() {
    	    assert.equal(tray.menu, menu);
	    var new_menu = new gui.Menu();
    	    tray.menu = new_menu;
    	    assert.equal(tray.menu, new_menu);
	    tray.menu = menu;
    	    assert.equal(tray.menu, menu);
    	});
    });

    describe('#.alticon , not implentment', function() {
    	// Get or Set the alternate (active) tray icon for Mac. It has no effects on Linux and Win.
    }); 

    var click_test = false;
    tray.on('click', function() {
    	click_test = true;
    });


    describe('#click()', function() {
    	if (process.platform == 'win32' || process.platform == 'linux') {
    	    tray._events.click();
	    
    	    it('should be catched on Windows and Linux', function() {
    		assert.equal(click_test, true);
    	    });
    	}
    });
    
    
    describe('#remove(), not implentment',function(){
    });

    
    // not sure what it means?
    // var tray2 = new gui.Tray({
    // 	title: 'Test Tray 2',
    // 	menu: menu
    // });
    
    // assert(
    // 	tray.menu == tray2.menu,
    // 	tray2.title == 'Test Tray 2',
    // 'Second Tray');
});
