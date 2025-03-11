#ifndef MAP_H
#define MAP_H

#include <vector>
#include <string>

template <typename T>
class Map {
public:
    // Constructors
    Map();
    Map(int w, int h);
    
    // Methods
    bool loadFromFile(const std::string& filename, const std::string& format);
    bool saveToFile(const std::string& filename, const std::string& format) const;
    T getData(int x, int y) const;
    void setData(int x, int y, T value);
    int getWidth(void) const;
    int getHeight(void) const;
    bool exportToImage(const std::string& filename, const std::string& format) const;
    void fillSinks(void);
    void applyScaling(const std::string& scale, double percentile = 0.5);

private:
    // Vars
    std::vector<std::vector<T>> mapData;
    int width, height;

    // Private Methods
    bool loadFromTXT(const std::string& filename);
    bool loadFromCSV(const std::string& filename);
    bool loadFromBin(const std::string& filename);

    bool saveToTXT(const std::string& filename) const;
    bool saveToCSV(const std::string& filename) const;
    bool saveToBin(const std::string& filename) const;

    bool exportToBW(const std::string& filename) const;
    bool exportToWB(const std::string& filename) const;
    // Cite DryWet
    bool exportToDryWet(const std::string& filename) const;
    bool exportToD8(const std::string& filename) const;
    bool exportToSeaFloor(const std::string& filename) const;

    bool is_sink(int x, int y);
};


// Specialised Vector Class
template <typename T>
class Map<std::vector<T>> {

public:
    // Constructors
    Map();
    Map(int w, int h);
    // Methods
    bool loadFromFile(const std::string& filename, const std::string& format);
    bool saveToFile(const std::string& filename, const std::string& format) const;
    std::vector<T>& getData(int x, int y);
    void setData(int x, int y, const std::vector<T>& value);
    int getWidth(void) const;
    int getHeight(void) const;



private:
    // Vars
    std::vector<std::vector<std::vector<T>>> mapData;
    int width, height;

    // Private Methods
    bool loadFromTXT(const std::string& filename);
    bool loadFromBin(const std::string& filename);

    bool saveToTXT(const std::string& filename) const;
    bool saveToBin(const std::string& filename) const;
    
};

#endif