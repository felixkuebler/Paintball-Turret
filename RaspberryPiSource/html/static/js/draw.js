const color = "#FF0000";
const fontName = "Arial";
const fontSize = 25;
const font = fontSize + "px " + fontName;
const lineWidth = 2;

const maxDegX = 180;
const maxDegY = 90;


function drawInfo(context, info){

    var width = context.canvas.width;
    var height = context.canvas.height;
    
    var infoPedding = 25;

    context.font = font;
    context.textAlign = "center";
    context.fillText(info, width/2, height-infoPedding);
    context.stroke();
}


function drawCrosshair(context){

    var width = context.canvas.width;
    var height = context.canvas.height;

    var horizontalLineLength = width/7;
    var verticalLineLength = height/7;

    var horizontalCrosshairLinelength = horizontalLineLength/5*3;
    var verticalCrosshairLineLength = verticalLineLength/5*3;

    context.fillStyle = color;
    context.globalAlpha = 1;

    //outer frame horizontal lines
    context.fillRect(width/5, height/5-lineWidth/2, horizontalLineLength, lineWidth);
    context.fillRect(width-width/5-horizontalLineLength, height/5-lineWidth/2, horizontalLineLength, lineWidth);
    context.fillRect(width/5, height/5*4-lineWidth/2, horizontalLineLength, lineWidth);
    context.fillRect(width-width/5-horizontalLineLength, height/5*4-lineWidth/2, horizontalLineLength, lineWidth);

    //outer frame vertical lines
    context.fillRect(width/5-lineWidth/2, height/5, lineWidth, verticalLineLength);
    context.fillRect(width/5-lineWidth/2, height-height/5-verticalLineLength, lineWidth, verticalLineLength);
    context.fillRect(width/5*4-lineWidth/2, height/5, lineWidth, verticalLineLength);
    context.fillRect(width/5*4-lineWidth/2, height-height/5-verticalLineLength, lineWidth, verticalLineLength);

    //inner frame horizontal lines
    context.fillRect(width/5*2, height/2-lineWidth/2, horizontalCrosshairLinelength, lineWidth);
    context.fillRect(width-width/5*2-horizontalCrosshairLinelength, height/2-lineWidth/2, horizontalCrosshairLinelength, lineWidth);

    //inner frame vertical lines
    context.fillRect(width/2-lineWidth/2, height/5*2, lineWidth, verticalCrosshairLineLength);
    context.fillRect(width/2-lineWidth/2, height-height/5*2-verticalCrosshairLineLength, lineWidth, verticalCrosshairLineLength);

    context.stroke();
}


function drawPosition(context, xPos, yPos){

    var width = context.canvas.width;
    var height = context.canvas.height;

    var dashPedding= 5;
    var dashLength = 25;
    var numberPedding = 25;

    var widthDeg = width/maxDegX;

    context.fillStyle = color;
    context.globalAlpha = 1;

    for(i=0; i<=maxDegX; i+=5){

        var positionShift = width/2+((i-(xPos+maxDegX/2))*widthDeg);
        var alphaValue = Math.abs(positionShift-(width/2))/2;

        if (alphaValue<1) alphaValue = 1;
        alphaValue = (40/alphaValue);
        if (alphaValue>1) alphaValue = 1;

        if (alphaValue<0.2) alphaValue = 0;

        context.globalAlpha = alphaValue;

        if ((i-maxDegX/2)%10 == 0){
            context.fillRect(positionShift-lineWidth/2, dashPedding, lineWidth, dashLength);

            context.font = font;
            context.textAlign = "center";
            context.fillText(i-maxDegX/2, positionShift, dashPedding+dashLength+numberPedding); 
        } else {
            context.fillRect(positionShift-lineWidth/2, dashPedding, lineWidth, dashLength/3*2);
        }
    }

    context.globalAlpha = 1;

    var path=new Path2D();
    path.moveTo((width/2)+widthDeg,dashPedding+dashLength+numberPedding+numberPedding);
    path.lineTo((width/2),dashPedding+dashLength+numberPedding+numberPedding/3);
    path.lineTo((width/2)-widthDeg,dashPedding+dashLength+numberPedding+numberPedding);
    context.fill(path);


    for(i=0; i<=maxDegY; i+=5){

        var positionShift = height/2+((i-(yPos+maxDegY/2))*widthDeg);

        var alphaValue = Math.abs(positionShift-(height/2));
        if (alphaValue<1) alphaValue = 1;
        alphaValue = (40/alphaValue);
        if (alphaValue>1) alphaValue = 1;

        if (alphaValue<0.2) alphaValue = 0;

        context.globalAlpha = alphaValue;

        if ((i-maxDegY/2)%10 == 0){
            context.fillRect(width-dashPedding-dashLength, positionShift-lineWidth/2, dashLength, lineWidth);

            context.font = font;
            context.textAlign = "center";
            context.fillText(maxDegY/2-i, width-dashPedding-dashLength-numberPedding, positionShift+fontSize/3); 
        } else {
            context.fillRect(width-dashPedding-dashLength, positionShift-lineWidth/2, dashLength/3*2, lineWidth);
        }
    }

    context.globalAlpha = 1;

    var path=new Path2D();
    path.moveTo(width-dashPedding-dashLength-numberPedding*3, (height/2)+widthDeg);
    path.lineTo(width-dashPedding-dashLength-numberPedding*2-numberPedding/3, (height/2));
    path.lineTo(width-dashPedding-dashLength-numberPedding*3, (height/2)-widthDeg);
    context.fill(path);

    context.stroke();
}


function drawTriggerToggle(context){
    var width = context.canvas.width;
    var height = context.canvas.height;

    context.strokeStyle = color;

    context.moveTo(width/2-15, height/2-10);
    context.lineTo(width/2-35, height/2-20);

    context.moveTo(width/2+15, height/2-10);
    context.lineTo(width/2+35, height/2-20);

    context.moveTo(width/2-15, height/2+10);
    context.lineTo(width/2-35, height/2+20);

    context.moveTo(width/2+15, height/2+10);
    context.lineTo(width/2+35, height/2+20);

    context.stroke();
}

function drawRotatedImage(context, image, x, y, w, h, degrees){
    context.save();
    context.translate(x+w/2, y+h/2);
    context.rotate(degrees*Math.PI/180.0);
    context.translate(-x-w/2, -y-h/2);
    context.drawImage(image, x, y, w, h);
    context.restore();
}