#include "VeicleStat.h"

// function that open the file and return the file pointer
int CheckStatisticFile(){
    CarFp = fopen("./Data/Car.txt", "r");
    TruckFp = fopen("./Data/Truck.txt", "r");
    MotorcycleFp = fopen("./Data/Motorcycle.txt", "r");
    if (CarFp == NULL)
    {
        printf("Error opening file Car.txt\n");
        return 0;
    }
    if (TruckFp == NULL)
    {
        printf("Error opening file Truck.txt\n");
        return 0;
    }
    if (MotorcycleFp == NULL)
    {
        printf("Error opening file Motorcycle.txt\n");
        return 0;
    }

    return 1;
}

// function that read line and retun car Statistic struct
struct VeicleStatistics *GetVeicleStaitistics(int type){
   
    struct VeicleStatistics *Statistics = malloc(sizeof(struct VeicleStatistics));
    FILE * fp;

    switch (type)
    {
    case CAR:
        fp = CarFp;
        break;
    case TRUCK:
        fp = TruckFp;
        break;
    case MOTORCYCLE:
        fp = MotorcycleFp;
        break;
    default:
        printf("Error: type not valid\n");
        return NULL;
        break;
    }


    // get file row number
    int row = 0;
    char c;
    for (c = getc(fp); c != EOF; c = getc(fp))
        if (c == '\n') // Increment count if this character is newline
            row++;
    
    // reset file pointer
    rewind(fp);

    // get random row
    int randomRow = rand() % row;

    // read randomRow avoiding - as separator
    int i = 0;
    char line[256];
    while (fgets(line, sizeof(line), fp))
    {
        if (i == randomRow)
        {
            sscanf(line, "%lf-%lf-%lf-%lf", &Statistics->maxSpeed, &Statistics->maxAcceleration, &Statistics->maxDeceleration, &Statistics->minDistance);
            break;
        }
        i++;
    }

    return Statistics;
}

// function that close the file
void CloseStatisticFile(){
    fclose(CarFp);
    fclose(TruckFp);
    fclose(MotorcycleFp);
}