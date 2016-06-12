from Foundation import NSAppleScript, NSDictionary

class WinManager:

    def close_window(self):
        src = '''
        global frontApp, frontAppName

        tell application "System Events"
            set frontApp to first application process whose frontmost is true
            set frontAppName to name of frontApp
            tell process frontAppName
                tell (1st window whose value of attribute "AXMain" is true)
                    click 1st button
                end tell
            end tell
        end tell
        '''

        s = NSAppleScript.alloc().initWithSource_(src)
        r, e = s.executeAndReturnError_(None)
        if e:
            print e['NSAppleScriptErrorBriefMessage']
            raise Exception(e['NSAppleScriptErrorBriefMessage'])