package {
  import flash.display.Sprite;
  import flash.external.ExternalInterface;
  import flash.system.Security;
  import flash.text.TextField;
  import flash.text.TextFormat;
  
  public class HelloWorld extends Sprite {
    
    public function HelloWorld() {
      Security.allowDomain('*');
      ExternalInterface.marshallExceptions = true;

      var myFormat:TextFormat = new TextFormat();
      myFormat.size = 32;

      var display_txt:TextField = new TextField();
      display_txt.defaultTextFormat = myFormat;
      display_txt.width = 300;
      display_txt.wordWrap = true;
      addChild(display_txt);

      if (ExternalInterface.available) {
        display_txt.text = 'ExternalInterface is available';
        try {
          ExternalInterface.call('out', 'result', 'success');
        } catch (e:Error) {
          display_txt.text = e.toString();
        }
      } else {
        display_txt.text = 'ExternalInterface is NOT available';
      }
    }

    private function ready():String {
      return 'success';
    }
  }
}