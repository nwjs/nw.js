/**
 * Created by haisongc on 14-4-23.
 */

import java.awt.*;
import java.awt.event.InputEvent;

public class Mouse {
    private Robot robot;
    public Mouse() throws AWTException {
        robot = new Robot();
    }
    public static void main(String[] args) throws Exception {
        Mouse m = new Mouse();
        if (args[0].equals("-h") || args[0].equals("--help")){
            m.help();
            return;
        } else {
            for (int i=0;i<args.length;++i){
                String[] tokens = args[i].split(",");
                if (tokens.length != 3){
                    m.help();
                    return ;
                }
                int x = new Integer(tokens[0]);
                int y = new Integer(tokens[1]);
                int holdTime = new Integer(tokens[2]);
                m.click(x,y,holdTime);
            }
        }
    }

    private void help(){
        String message =
                "Usage: java -jar Mouse.jar x1,y1,holdTime1 x2,y2,holdTime2 ...\n" +
                "       java -jar Mouse.jar -h\n" +
                "       java -jar Mouse.jar --help";
        System.out.println(message);
    }

    private void click(int x,int y,int holdTime) throws InterruptedException {
        robot.mouseMove(x,y);
        Thread.sleep(20);
        robot.mousePress(InputEvent.BUTTON1_MASK);
        Thread.sleep(20);
        robot.mouseRelease(InputEvent.BUTTON1_MASK);
        Thread.sleep(holdTime);
        return;
    }
}
