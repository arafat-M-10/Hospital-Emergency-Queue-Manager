#include <stdio.h>
#include <string.h>

#define MAX 100

//================ Structure ===================

struct Patient
{
    int id;
    char name[50];
    int age;
    int emergency;      // 1=Critical 2=Serious 3=Normal
    int arrival;
};

struct Patient patient[MAX];

int totalPatient = 0;
int arrivalCount = 1;

//================ Function Prototype ===================

void addPatient();
void displayPatients();
void sortPatients();
void treatPatient();
void searchPatient();
void deletePatient();
void updatePatient();

//================ Main Function ===================

int main()
{
    int choice;

    while(1)
    {
        printf("\n====================================");
        printf("\n HOSPITAL EMERGENCY QUEUE MANAGER");
        printf("\n====================================");

        printf("\n1. Add Patient");
        printf("\n2. Display Waiting List");
        printf("\n3. Treat Next Patient");
        printf("\n4. Search Patient");
        printf("\n5. Delete Patient");
        printf("\n6. Update Patient");
        printf("\n7. Exit");

        printf("\n\nEnter Choice : ");
        scanf("%d",&choice);

        switch(choice)
        {
            case 1:
                addPatient();
                break;

            case 2:
                displayPatients();
                break;

            case 3:
                treatPatient();
                break;

            case 4:
                searchPatient();
                break;

            case 5:
                deletePatient();
                break;

            case 6:
                updatePatient();
                break;

            case 7:
                printf("\nThank You.\n");
                return 0;

            default:
                printf("\nInvalid Choice!\n");
        }
    }

    return 0;
}

//================ Add Patient ===================

void addPatient()
{
    if(totalPatient == MAX)
    {
        printf("\nQueue is Full!\n");
        return;
    }

    printf("\nEnter Patient ID : ");
    scanf("%d",&patient[totalPatient].id);

    printf("Enter Patient Name : ");
    scanf(" %[^\n]",patient[totalPatient].name);

    printf("Enter Age : ");
    scanf("%d",&patient[totalPatient].age);

    printf("\nEmergency Level");
    printf("\n1 = Critical");
    printf("\n2 = Serious");
    printf("\n3 = Normal");

    printf("\nEnter Emergency Level : ");
    scanf("%d",&patient[totalPatient].emergency);

    patient[totalPatient].arrival = arrivalCount;

    arrivalCount++;

    totalPatient++;

    sortPatients();

    printf("\nPatient Added Successfully.\n");
}

//================ Sort Patient ===================

void sortPatients()
{
    int i,j;

    struct Patient temp;

    for(i=0;i<totalPatient-1;i++)
    {
        for(j=0;j<totalPatient-i-1;j++)
        {
            if(patient[j].emergency > patient[j+1].emergency)
            {
                temp=patient[j];
                patient[j]=patient[j+1];
                patient[j+1]=temp;
            }

            else if(patient[j].emergency==patient[j+1].emergency)
            {
                if(patient[j].arrival>patient[j+1].arrival)
                {
                    temp=patient[j];
                    patient[j]=patient[j+1];
                    patient[j+1]=temp;
                }
            }



        }
    }
}
//================ Display Patient ===================

void displayPatients()
{
    int i;

    if(totalPatient==0)
    {
        printf("\nNo Patient Found!\n");
        return;
    }

    printf("\n==============================================================");
    printf("\nID\tName\t\tAge\tEmergency\tArrival");
    printf("\n==============================================================");

    for(i=0;i<totalPatient;i++)
    {
        printf("\n%d\t",patient[i].id);

        printf("%s\t\t",patient[i].name);

        printf("%d\t",patient[i].age);

        if(patient[i].emergency==1)
            printf("Critical\t");

        else if(patient[i].emergency==2)
            printf("Serious\t\t");

        else
            printf("Normal\t\t");

        printf("%d",patient[i].arrival);
    }

    printf("\n==============================================================\n");
}

//================ Treat Patient ===================

void treatPatient()
{
    int i;

    if(totalPatient==0)
    {
        printf("\nNo Patient in Queue!\n");
        return;
    }

    printf("\n========== Patient Information ==========");

    printf("\nPatient ID : %d",patient[0].id);
    printf("\nName       : %s",patient[0].name);
    printf("\nAge        : %d",patient[0].age);

    if(patient[0].emergency==1)
        printf("\nEmergency  : Critical");

    else if(patient[0].emergency==2)
        printf("\nEmergency  : Serious");

    else
        printf("\nEmergency  : Normal");

    printf("\nArrival No : %d",patient[0].arrival);

    printf("\n\nTreatment Completed Successfully.\n");

    // Queue Shift
    for(i=0;i<totalPatient-1;i++)
    {
        patient[i]=patient[i+1];
    }

    totalPatient--;

    printf("\nPatient Removed From Queue.\n");
}
//================ Search Patient ===================

void searchPatient()
{
    int id,i,found=0;

    printf("\nEnter Patient ID : ");
    scanf("%d",&id);

    for(i=0;i<totalPatient;i++)
    {
        if(patient[i].id==id)
        {
            found=1;

            printf("\nPatient Found");
            printf("\nID        : %d",patient[i].id);
            printf("\nName      : %s",patient[i].name);
            printf("\nAge       : %d",patient[i].age);

            if(patient[i].emergency==1)
                printf("\nEmergency : Critical");
            else if(patient[i].emergency==2)
                printf("\nEmergency : Serious");
            else
                printf("\nEmergency : Normal");

            printf("\nArrival   : %d\n",patient[i].arrival);

            break;
        }
    }

    if(found==0)
    {
        printf("\nPatient Not Found!\n");
    }
}

//================ Delete Patient ===================

void deletePatient()
{
    int id,i,j,found=0;

    printf("\nEnter Patient ID to Delete : ");
    scanf("%d",&id);

    for(i=0;i<totalPatient;i++)
    {
        if(patient[i].id==id)
        {
            found=1;

            for(j=i;j<totalPatient-1;j++)
            {
                patient[j]=patient[j+1];
            }

            totalPatient--;

            printf("\nPatient Deleted Successfully.\n");

            break;
        }
    }

    if(found==0)
    {
        printf("\nPatient Not Found!\n");
    }
}

//================ Update Patient ===================

void updatePatient()
{
    int id,i,found=0;

    printf("\nEnter Patient ID to Update : ");
    scanf("%d",&id);

    for(i=0;i<totalPatient;i++)
    {
        if(patient[i].id==id)
        {
            found=1;

            printf("\nEnter New Name : ");
            scanf(" %[^\n]",patient[i].name);

            printf("Enter New Age : ");
            scanf("%d",&patient[i].age);

            printf("\nEmergency Level");
            printf("\n1 = Critical");
            printf("\n2 = Serious");
            printf("\n3 = Normal");

            printf("\nEnter New Emergency Level : ");
            scanf("%d",&patient[i].emergency);

            // Priority আবার ঠিক করার জন্য Sort
            sortPatients();

            printf("\nPatient Updated Successfully.\n");

            break;
        }
    }

    if(found==0)
    {
        printf("\nPatient Not Found!\n");
    }
}
