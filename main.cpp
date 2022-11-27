#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

class Grade {
public:
    double grade;
    Grade *next;
};

class Subject {
public:
    int id;
    string name;
    double average;
    Grade *grades;
    Subject *next;
};

class Preset {
public:
    int id;
    string name;
    Subject *subjects;
    Preset *next;
};

Subject *inputSubjects();

Preset *createPreset();

void addToHeadPreset(Preset **head, Preset *preset);

void getPredefinedPresets();

void selectPreset();

void showMenu();

void enterGrades();

void gradesOverview();

void computeAverages();

double computeGenAverage();

void deletePreset();

void freeAll();

Preset *presetHead = nullptr;
Subject *subjHead = nullptr;

int main() {
    getPredefinedPresets();
    selectPreset();

    // Runs the program infinitely, ends only when user chooses to exit using the options menu
    while (true) {
        showMenu();
        int choice;
        do {
            cout << "--> ";
            cin >> choice;
            cin.ignore();
        } while (choice < 1 || choice > 6);

        switch (choice) {
            case 1:
                enterGrades();
                break;
            case 2:
                gradesOverview();
                break;
            case 3:
                computeAverages();
                break;
            case 4:
                selectPreset();
                break;
            case 5:
                deletePreset();
                break;
            default:
                freeAll();
                return 0;
        }
    }
}

Subject *inputSubjects() {
    Subject *head = nullptr, *newSubject;
    int count = 1, subjSize;

    // Get number of subjects from user
    do {
        cout << "Number of Subjects: ";
        cin >> subjSize;
        cin.ignore();
    } while (subjSize < 1);

    // Creates subject nodes based on the number of subjects
    do {

        // Creating new subject node
        newSubject = new Subject();
        newSubject->id = count;
        newSubject->next = nullptr;

        // Get subject name from user and insert into node
        cout << "Enter name of Subject " << count << ": ";
        getline(cin, newSubject->name);

        // Add subject to head
        if (head == nullptr) {
            head = newSubject;
        } else {
            Subject *temp = head;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newSubject;
        }

        // Increase count tracker
        count++;

    } while (count <= subjSize);
    cout << endl;

    return head;
}

Preset *createPreset() {
    cout << "Creating new preset..." << endl << endl;

    // Creating new preset node
    Preset *preset = new Preset();
    preset->next = nullptr;

    // Get preset name from user
    cout << "Enter name for preset: ";
    getline(cin, preset->name);

    // Get subjects and insert into preset node
    Subject *subjects = inputSubjects();
    preset->subjects = subjects;

    return preset;
}

void addToHeadPreset(Preset **head, Preset *preset) {

    // If head of preset is empty, replace with arg preset
    if (*head == nullptr) {
        preset->id = 1;
        *head = preset;
    } else {
        // Otherwise, traverse through the presets until the next node is free and then insert it
        Preset *temp = *head;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        preset->id = temp->id + 1;
        temp->next = preset;
    }
}

void getPredefinedPresets() {
    ifstream presetFile("presets.txt");
    Preset *predefined = nullptr, *newPredefined;
    string contents;

    if (presetFile.is_open()) {
        while (getline(presetFile, contents)) {

            // New node
            newPredefined = new Preset();
            newPredefined->next = nullptr;

            // Parse name and insert to new node
            stringstream ss(contents);
            getline(ss, newPredefined->name, ':');
            ss.clear();

            // Parse subjects and insert to new node
            string subjects = &contents[contents.find(':') + 1];
            ss.str(subjects);
            Subject *subj_head = nullptr, *subj;
            while (ss.good()) {
                subj = new Subject();
                subj->next = nullptr;
                getline(ss, subj->name, ',');

                // Insert each subject to a subject node
                if (subj_head == nullptr) {
                    subj->id = 1;
                    subj_head = subj;
                } else {
                    Subject *find = subj_head;
                    while (find->next != nullptr) {
                        find = find->next;
                    }
                    subj->id = find->id + 1;
                    find->next = subj;
                }
            }

            // Insert head of subject node to the current preset node
            newPredefined->subjects = subj_head;

            // Add current preset node to the main preset head
            addToHeadPreset(&predefined, newPredefined);

            if (predefined == nullptr) {
                cout << "Empty Predefined!";
            }
        }

        // Replace head for preset node with predefined presets
        presetHead = predefined;

        // Close file
        presetFile.close();
    } else {
        cout << "No saved 'presets.txt' available. Skipped!" << endl;
    }

}

void selectPreset() {

    // Creates a new preset if the head for the preset node is empty
    if (presetHead == nullptr) {
        cout << "You dont have any presets yet." << endl;
        Preset *preset = createPreset();
        addToHeadPreset(&presetHead, preset);
        subjHead = preset->subjects;
    } else {

        // Otherwise, let user choose from the available presets
        Preset *chosen = nullptr;
        int choice;
        do {
            cout << "Choose a preset:\n";

            // Show all presets
            cout << "[0] New preset" << endl;
            for (Preset *temp = presetHead; temp != nullptr; temp = temp->next) {
                cout << "[" << temp->id << "] " << temp->name << endl;
            }
            cout << "--> ";
            cin >> choice;
            cin.ignore();

            if (choice == 0) {
                chosen = createPreset();
                addToHeadPreset(&presetHead, chosen);
            } else {
                // Compare choice with every id on all preset
                for (Preset *temp = presetHead; temp != nullptr; temp = temp->next) {
                    if (temp->id == choice) {
                        chosen = temp;
                    }
                }
            }
        } while (chosen == nullptr);

        // Replace head of the subject node with the subjects of the selected preset
        subjHead = chosen->subjects;
    }
}

void showMenu() {
    cout << "What do you want to do? (Average Calculator)" << endl;
    cout << "[1] Enter Grades" << endl;
    cout << "[2] Grades Overview" << endl;
    cout << "[3] Compute Averages" << endl;
    cout << "[4] Change Preset" << endl;
    cout << "[5] Delete Preset" << endl;
    cout << "[6] Exit" << endl;
}

void enterGrades() {

    // Get quarter size from user
    int quarters;
    do {
        cout << "How many quarters does each subject have? ";
        cin >> quarters;
        cin.ignore();
    } while (quarters < 1);

    // Inserting grades by subject
    for (Subject *temp = subjHead; temp != nullptr; temp = temp->next) {

        // Creating new node for grade
        Grade *grade = nullptr, *add;

        // Grades are inserted based on how many quarters
        for (int i = 0; i < quarters; i++) {
            add = new Grade();
            cout << "Enter Q" << i + 1 << " grade for " << temp->name << ": ";
            cin >> add->grade;
            cin.ignore();

            // Add grade to head
            if (grade == nullptr) {
                grade = add;
            } else {
                Grade *find = grade;
                while (find->next != nullptr) {
                    find = find->next;
                }
                find->next = add;
            }
        }
        cout << endl;

        // Insert head for grade to the subject node
        temp->grades = grade;
    }
}

void gradesOverview() {

    // Check if there are grades set by user
    for (Subject *temp = subjHead; temp != nullptr; temp = temp->next) {
        if (temp->grades == nullptr) {
            cout << "Grades not entered yet!" << endl << endl;
            return;
        }
    }

    // Continue if there is grades entered
    cout << "Grades Overview" << endl;
    cout << "======================================" << endl;

    // Display general average only if it's set
    if (computeGenAverage() != 0) {
        cout << "General Average: " << computeGenAverage() << endl;
    }

    for (Subject *temp = subjHead; temp != nullptr; temp = temp->next) {

        // Show grades by subject
        cout << "[" << temp->id << "] " << temp->name << endl;
        cout << "\tGrades: ";
        for (Grade *get = temp->grades; get != nullptr; get = get->next) {
            cout << get->grade << "\t";
        }

        // Display subject average only if it's set
        if (temp->average != 0) {
            cout << endl << "\tAverage: " << temp->average;
        }

        cout << endl;
    }
    cout << "======================================" << endl;
}


void computeAverages() {

    // Check if there are grades entered to compute average
    for (Subject *temp = subjHead; temp != nullptr; temp = temp->next) {
        if (temp->grades == nullptr) {
            cout << "Grades not entered yet!" << endl << endl;
            return;
        }
    }

    // Compute Subject Averages
    cout << "Subject Averages:" << endl;
    cout << "======================================" << endl;
    for (Subject *subjTemp = subjHead; subjTemp != nullptr; subjTemp = subjTemp->next) {

        // Showing Subject names and id
        cout << "[" << subjTemp->id << "] " << subjTemp->name << ": ";

        // Computing average, displaying the result, and saving it to node
        double average = 0, count = 0;
        for (Grade *gradeTemp = subjTemp->grades; gradeTemp != nullptr; gradeTemp = gradeTemp->next) {
            average += gradeTemp->grade;
            count++;
        }
        average /= count;
        cout << average << endl;
        subjTemp->average = average;
    }

    cout << "======================================" << endl;
    cout << "General Average: " << computeGenAverage() << endl << endl;
}

double computeGenAverage() {
    double average = 0, count = 0;
    for (Subject *temp = subjHead; temp != nullptr; temp = temp->next) {
        average += temp->average;
        count++;
    }
    average /= count;

    return average;
}

void deletePreset() {
    int choice;

    // Show all presets
    cout << "Choose a preset to delete: " << endl;
    for (Preset *temp = presetHead; temp != nullptr; temp = temp->next) {
        cout << "[" << temp->id << "] " << temp->name << endl;
    }
    cout << "--> ";
    cin >> choice;
    cin.ignore();

    // Delete preset head if the id is same as user choice
    int current = 0;
    Preset *tmp = presetHead;
    if (tmp->id == choice) {

        if (presetHead->subjects == subjHead) {
            current = 1;
        }

        // Free Subject nodes of the head Preset, including its Grade nodes
        for (Subject *subjTemp = presetHead->subjects, *subjSave; subjTemp != nullptr; subjTemp = subjSave) {

            for (Grade *gradeTemp = subjTemp->grades, *gradeSave; gradeTemp != nullptr; gradeTemp = gradeSave) {
                gradeSave = gradeTemp->next;
                free(gradeTemp);
            }
            subjSave = subjTemp->next;
            free(subjTemp);
        }

        // Set the next preset to the next of this to-be-deleted preset, otherwise null
        if (tmp->next != nullptr) {
            presetHead = tmp->next;
        } else {
            presetHead = nullptr;
        }

        // Delete the preset finally
        free(tmp);

        // Force user to select preset again if the subjects are being used
        if (current == 1) {
            selectPreset();
            return;
        }
    }

    // Otherwise, compare choice with every other preset
    for (Preset *temp = presetHead, *save; temp != nullptr; temp = temp->next) {

        if (temp->next != nullptr) {

            // Delete next preset if the id is same as user choice
            if (temp->next->id == choice) {

                save = temp->next;
                if (save->subjects == subjHead) {
                    current = 1;
                }

                // Free Subject nodes of the next Preset, including its Grade nodes
                for (Subject *subjTemp = save->subjects, *subjSave; subjTemp != nullptr; subjTemp = subjSave) {

                    for (Grade *gradeTemp = subjTemp->grades, *gradeSave; gradeTemp != nullptr; gradeTemp = gradeSave) {
                        gradeSave = gradeTemp->next;
                        free(gradeTemp);
                    }
                    subjSave = subjTemp->next;
                    free(subjTemp);
                }

                // Set the next preset to the next of this to-be-deleted preset, otherwise null
                if (temp->next->next != nullptr) {
                    temp->next = temp->next->next;
                } else {
                    temp->next = nullptr;
                }

                // Delete the preset finally
                free(save);

                // Force user to select preset again if the subjects are being used
                if (current == 1) {
                    selectPreset();
                }
            }
        }
    }
}

void freeAll() {

    // Free each of preset list containing the subject list and the grades list
    for (Preset *presetTemp = presetHead, *save1; presetTemp != nullptr; presetTemp = save1) {
        for (Subject *subjTemp = presetTemp->subjects, *save2; subjTemp != nullptr; subjTemp = save2) {
            for (Grade *gradeTemp = subjTemp->grades, *save3; gradeTemp != nullptr; gradeTemp = save3) {
                save3 = gradeTemp->next;
                free(gradeTemp);
            }
            save2 = subjTemp->next;
            free(subjTemp);
        }
        save1 = presetTemp->next;
        free(presetTemp);
    }
}