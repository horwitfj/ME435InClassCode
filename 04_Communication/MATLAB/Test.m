direction = "LEFT"
previewPos = 2;
if direction == "LEFT"
    previewPos = previewPos - 1;
elseif direction == "RIGHT"
    previewPos = previewPos + 1;
end
previewPos = mod(previewPos-1, 7) + 1