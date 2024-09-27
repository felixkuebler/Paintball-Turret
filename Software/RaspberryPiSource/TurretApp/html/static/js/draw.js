const color = "#FFFFFF";
const fontName = "Arial";

const lineWidth = 2;

const maxDegX = 180;
const maxDegY = 90;

var fontSize = window.screen.height/100*4;
var font = fontSize + "px " + fontName;

var dashLength = window.screen.height/100*4;

var dashPeddingX= window.screen.height/100*2;
var dashPeddingY= window.screen.width/100*1;

var numberPeddingX = window.screen.height/100*4;
var numberPeddingY = window.screen.width/100*2;


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

    var verticalCrosshairLineLength = verticalLineLength/6*3;
    //var horizontalCrosshairLinelength = horizontalLineLength/5*3;
    var horizontalCrosshairLinelength = verticalCrosshairLineLength;

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
    context.fillRect(width/2-horizontalCrosshairLinelength-horizontalCrosshairLinelength/4, height/2-lineWidth/2, horizontalCrosshairLinelength, lineWidth);
    context.fillRect(width/2+horizontalCrosshairLinelength/4, height/2-lineWidth/2, horizontalCrosshairLinelength, lineWidth);

    //inner frame vertical lines
    context.fillRect(width/2-lineWidth/2, height/2-verticalCrosshairLineLength-verticalCrosshairLineLength/4, lineWidth, verticalCrosshairLineLength);
    context.fillRect(width/2-lineWidth/2, height/2+verticalCrosshairLineLength/4, lineWidth, verticalCrosshairLineLength);

    context.stroke();
}


function drawPositionFeedbackFullScreen(context, xPos, yPos){

    var width = context.canvas.width;
    var height = context.canvas.height;

    var widthDeg = width/maxDegX;

    context.fillStyle = color;
    context.globalAlpha = 1;

    for(i=0; i<=maxDegX; i+=5){

        var positionShift = width/2+((i-(xPos+maxDegX/2))*widthDeg);

        // only draw if within range of crosshair
        if (positionShift > width/5 && positionShift < width/5*4){

            // fade out to screen edges
            var alphaValue = Math.abs(positionShift-(width/2))/2;
            if (alphaValue<1) alphaValue = 1;
            alphaValue = (40/alphaValue);
            if (alphaValue>1) alphaValue = 1;


            context.globalAlpha = alphaValue;

            if ((i-maxDegX/2)%10 == 0){
                context.fillRect(positionShift-lineWidth/2, dashPeddingX, lineWidth, dashLength);

                context.font = font;
                context.textAlign = "center";
                context.fillText(i-maxDegX/2, positionShift, dashPeddingX+dashLength+numberPeddingX); 
            } else {
                context.fillRect(positionShift-lineWidth/2, dashPeddingX, lineWidth, dashLength/3*2);
            }
        }
    }

    context.globalAlpha = 1;

    var path=new Path2D();
    path.moveTo((width/2)+widthDeg,dashPeddingX+dashLength+numberPeddingX+numberPeddingX);
    path.lineTo((width/2),dashPeddingX+dashLength+numberPeddingX+numberPeddingX/3);
    path.lineTo((width/2)-widthDeg,dashPeddingX+dashLength+numberPeddingX+numberPeddingX);
    context.fill(path);


    for(i=0; i<=maxDegY; i+=5){

        var positionShift = height/2+((i-(yPos+maxDegY/2))*widthDeg);

        // only draw if within range of crosshair
        if (positionShift > height/5 && positionShift < height/5*4){

            // fade out to screen edges
            var alphaValue = Math.abs(positionShift-(height/2));
            if (alphaValue<1) alphaValue = 1;
            alphaValue = (40/alphaValue);
            if (alphaValue>1) alphaValue = 1;

            context.globalAlpha = alphaValue;

            if ((i-maxDegY/2)%10 == 0){
                context.fillRect(width-dashPeddingY-dashLength, positionShift-lineWidth/2, dashLength, lineWidth);

                context.font = font;
                context.textAlign = "center";
                context.fillText(maxDegY/2-i, width-dashPeddingY-dashLength-numberPeddingY, positionShift+fontSize/3); 
            } else {
                context.fillRect(width-dashPeddingY-dashLength+dashLength/3, positionShift-lineWidth/2, dashLength/3*2, lineWidth);
            }
        }
    }

    context.globalAlpha = 1;

    var path=new Path2D();
    path.moveTo(width-dashPeddingY-dashLength-numberPeddingY*3, (height/2)+widthDeg);
    path.lineTo(width-dashPeddingY-dashLength-numberPeddingY*2-numberPeddingY/3, (height/2));
    path.lineTo(width-dashPeddingY-dashLength-numberPeddingY*3, (height/2)-widthDeg);
    context.fill(path);

    context.stroke();
}


function drawPositionFeebackOverview(positionCanvasX, positionCanvasY, xPos, yPos)
{
    var contextX = positionCanvasX.getContext('2d');

    // get scaled canvas from image size
    var canvasScale = imageBottomX.naturalHeight/imageBottomX.naturalWidth;
    positionCanvasX.height = positionCanvasX.width*canvasScale;

    contextX.drawImage(imageBottomX, 0, 0, positionCanvasX.width, positionCanvasX.height);
    drawRotatedImage(contextX, imageTopX, 36, 0, positionCanvasX.width, positionCanvasX.height, xPos);


    var contextY = positionCanvasY.getContext('2d');

    // get scaled canvas from image size
    var canvasScale = imageTopY.naturalHeight/imageTopY.naturalWidth;
    var imageHeight = positionCanvasY.width*canvasScale;
    positionCanvasY.height = positionCanvasY.width*canvasScale/2*3;

    drawRotatedImage(contextY, imageBottomY, 25, imageHeight/2 -80, positionCanvasY.width, imageHeight, yPos);
    contextY.drawImage(imageTopY, 0, imageHeight/2, positionCanvasY.width, imageHeight); 
    drawRotatedImage(contextY, imageTopYCamera, 35, imageHeight/2 -80, positionCanvasY.width, imageHeight, yPos);
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