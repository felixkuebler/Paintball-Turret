const color = "#FFFFFF";
const fontName = "Arial";

const lineWidth = 2;

const maxDegX = 720;
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

    var widthDeg = 4*width/maxDegX;

    context.fillStyle = color;
    context.globalAlpha = 1;

    // add offset to repeat the number pattern infinite
    xPos -= maxDegX/2 * Math.trunc((xPos+Math.sign(xPos)*maxDegX/4)/(maxDegX/2));
    
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
               
                if (i > 3*maxDegX/4) {
                    context.fillText(i-maxDegX, positionShift, dashPeddingX+dashLength+numberPeddingX); 
                }
                else if (i < maxDegX/4) {
                    context.fillText(i, positionShift, dashPeddingX+dashLength+numberPeddingX); 
                }
                else {
                    context.fillText(i-maxDegX/2, positionShift, dashPeddingX+dashLength+numberPeddingX); 
                }
                                
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
