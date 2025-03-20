import { Application, Graphics, Container } from "pixi.js";
import { Button } from "@pixi/ui";

(async () => {
    const app = new Application();

    await app.init({
        resizeTo: window,
        backgroundColor: 'white',
    });

    app.canvas.style.position = 'absolute';

    const rectangle = new Graphics()
        .rect(200, 200, 100, 150)
        .fill({
            color: 'black',
            alpha: 0.9
        });

    

    app.stage.addChild(rectangle);

    rectangle.eventMode = 'static';
    rectangle.cursor = 'pointer';

    const container = new Container();
    const button = new Button(
         new Graphics()
             .rect(0, 0, 100, 50, 15)
             .fill('black')
    );
   
    button.onPress.connect(() => console.log('onPress'));
   
    container.addChild(button.view);

    app.stage.addChild(container);

    document.body.appendChild(app.canvas);

})();

