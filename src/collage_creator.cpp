#include <collage_creator.h>


Mat resize_and_pan(const Mat& img, int cell_side, const Scalar& bgcolor) {
    Mat resizedImg;
    double aspectRatio = static_cast<double>(img.cols) / img.rows;

    int newWidth = cell_side;
    int newHeight = static_cast<int>(newWidth / aspectRatio);

    if (newHeight > cell_side) {
        newHeight = cell_side;
        newWidth = static_cast<int>(newHeight * aspectRatio);
    }

    resize(img, resizedImg, Size(newWidth, newHeight));

    Mat paddedImg(cell_side, cell_side, img.type(), bgcolor);

    int xOffset = (cell_side - newWidth) / 2;
    int yOffset = (cell_side - newHeight) / 2;

    resizedImg.copyTo(paddedImg(Rect(xOffset, yOffset, newWidth, newHeight)));

    return paddedImg;
}


void create_collage(const std::string& folderPath, int canvasWidth, int canvasHeight, int images_count) {
    std::vector<Mat> images;

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            std::string filePath = entry.path().string();
            Mat img = imread(filePath);

            if (img.empty()) {
                std::cerr << "Ошибка чтения изображения: " << filePath << std::endl;
                continue;
            }

            images.push_back(img);
        }
    }

    if (images.empty()) {
        throw std::runtime_error("Нет изображений в указанной папке.");
    }

    int bestCellSide = 0, bestRows = 0, bestCols = 0;
    double minEmptySpace = std::numeric_limits<double>::max();

    for (int rows = 1; rows <= images_count; ++rows) {
        int cols = static_cast<int>(std::ceil(static_cast<double>(images_count) / rows));
        int cellWidth = canvasWidth / cols;
        int cellHeight = canvasHeight / rows;
        int cellSide = std::min(cellWidth, cellHeight);

        double usedWidth = cellSide * cols;
        double usedHeight = cellSide * rows;
        double emptySpace = (canvasWidth * canvasHeight) - (usedWidth * usedHeight);

        if (emptySpace < minEmptySpace) {
            minEmptySpace = emptySpace;
            bestCellSide = cellSide;
            bestRows = rows;
            bestCols = cols;
        }
    }

    Mat collage(canvasHeight, canvasWidth, CV_8UC3, Scalar(255, 255, 255));

    for (size_t i = 0; i < images.size() && i < static_cast<size_t>(bestRows * bestCols); ++i) {
        int row = i / bestCols;
        int col = i % bestCols;

        Mat resizedImg = resize_and_pan(images[i], bestCellSide);

        int xOffset = (canvasWidth - (bestCols * bestCellSide)) / 2 + col * bestCellSide;
        int yOffset = (canvasHeight - (bestRows * bestCellSide)) / 2 + row * bestCellSide;

        Rect roi(xOffset, yOffset, bestCellSide, bestCellSide);
        resizedImg.copyTo(collage(roi));
    }

    std::string outputFileName = "collage.jpg";
    imwrite(outputFileName, collage);

    std::cout << "Коллаж успешно создан: " << outputFileName << std::endl;
}