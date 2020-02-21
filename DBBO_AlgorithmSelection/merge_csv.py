with open('merge.csv', 'w') as writer:
    with open('DATA_PERFORMANCES/features_ordered.csv') as old_reader:
        with open('gen.csv') as new_reader:
            for old_line, new_line in zip(old_reader, new_reader):
                writer.write(old_line.strip() + ',' + new_line.strip() + '\n')
