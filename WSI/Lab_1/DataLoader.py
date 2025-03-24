class DataLoader:

    def __init__(self, train_data_path, train_labels_path, test_data_path, test_labels_path):
        self.train_data_path = train_data_path
        self.train_labels_path = train_labels_path
        self.test_data_path = test_data_path
        self.test_labels_path = test_labels_path

    def check_data(self):
        print("Checking data...", flush=True)

        train_data_length = 0
        try:
            with open(self.train_data_path, 'r') as file:
                train_data_length = len(file.readlines())
                print("train data opened successfully", flush=True)
        except FileNotFoundError:
            print("train data file not found or cannot be opened", flush=True)

        train_labels_length = 0
        try:
            with open(self.train_labels_path, 'r') as file:
                train_labels_length = len(file.readlines())
                print("train labels opened successfully", flush=True)
        except FileNotFoundError:
            print("train labels file not found or cannot be opened", flush=True)

        test_data_length = 0
        try:
            with open(self.test_data_path, 'r') as file:
                test_data_length = len(file.readlines())
                print("test data opened successfully", flush=True)
        except FileNotFoundError:
            print("test data file not found or cannot be opened", flush=True)

        test_labels_length = 0
        try:
            with open(self.test_labels_path, 'r') as file:
                test_labels_length = len(file.readlines())
                print("test labels opened successfully", flush=True)
        except FileNotFoundError:
            print("test labels file not found or cannot be opened", flush=True)

        if (train_data_length != train_labels_length) or (test_data_length != test_labels_length):
            return False
        else:
            return True

    def load_data(self):
        data_valid = self.check_data()

        if not data_valid:
            print("Data loading failed", flush=True)
            return None

        train_data = []
        train_labels = []
        test_data = []
        test_labels = []

        with open(self.train_data_path, 'r') as file:
            for raw in file:
                line = raw.strip().split(',')
                inputs = [float(line[i]) for i in range(len(line))]
                train_data.append(inputs)

        with open(self.train_labels_path, 'r') as file:
            for raw in file:
                line = raw.strip().split(',')
                answers = [float(line[i]) for i in range(len(line))]
                train_labels.append(answers)

        with open(self.test_data_path, 'r') as file:
            for raw in file:
                line = raw.strip().split(',')
                inputs = [float(line[i]) for i in range(len(line))]
                test_data.append(inputs)

        with open(self.test_labels_path, 'r') as file:
            for raw in file:
                line = raw.strip().split(',')
                answers = [float(line[i]) for i in range(len(line))]
                test_labels.append(answers)

        return train_data, train_labels, test_data, test_labels
