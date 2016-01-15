from Xlib import display, X, protocol, Xatom
import time

class WinManager:

    def __init__(self):
        self.display = display.Display()
        self.root = self.display.screen().root

    def _create_window(self, wid):
        return self.display.create_resource_object('window', wid)

    def _get_property(self, atom, win=None):
        if not win: win = self.root
        return win.get_full_property(self.display.get_atom(atom), X.AnyPropertyType).value

    def _set_property(self, ctype, data, win):
        data = (data + [0] * (5 - len(data)))[:5]
        ev = protocol.event.ClientMessage(window=win, client_type=self.display.get_atom(ctype), data=(32, data))
        mask = X.SubstructureRedirectMask | X.SubstructureNotifyMask
        self.root.send_event(ev, event_mask=mask)

    def close_window(self):
        win = self._create_window(self._get_property('_NET_ACTIVE_WINDOW')[0])
        self._set_property('_NET_CLOSE_WINDOW', [int(time.mktime(time.localtime())), 1], win)
        self.display.flush()