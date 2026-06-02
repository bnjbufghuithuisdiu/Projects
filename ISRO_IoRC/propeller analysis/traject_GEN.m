clear; clc;









for x = 0.11:-delX:0
    theta=minTheta;
    %theta boundary
    
    while vError<convergentThreshold
        h=hPrev+tan(theta)*delX;
        vDrop=math.sqrt(2*g*hold);
        vSlide=math.sqrt(2*g*(h-m*(0.11-x)));
        vError=vDrop-vSlide;
        %theta increment
        if vError>0
            theta=theta+1;
        else
            theta=theta-1;
        end
    end
end
