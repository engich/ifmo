import subprocess
import csv

dimensions = [16, 20, 32, 40, 50]

dummy_params = range(0, 21, 5)
neutrality_params = range(0, 21, 5)
epistasis_params = range(0, 26, 5)
ruggedness_params = range(0, 101, 5)

headers = ['a_walk',
           'acor_1.rho', 'acor_2.rho', 'acor_3.rho', 'acor_1.phi', 'acor_2.phi', 'acor_3.phi',
           'dos.min', 'dos.max', 'dos.mean', 'dos.std',
           'fdc_cor',
           'fn_cor',
           'n_deg.min', 'n_deg.max', 'n_deg.mean', 'n_deg.std',
           'n_walk.min', 'n_walk.max', 'n_walk.mean', 'n_walk.std']

with open('gen.csv', 'w', newline='') as csvfile:
    spamwriter = csv.writer(csvfile)
    spamwriter.writerow(headers)

    for dummy_param in dummy_params:
        for neutrality_param in neutrality_params:
            for epistasis_param in epistasis_params:
                for ruggedness_param in ruggedness_params:
                    for dimension in dimensions:
                        if neutrality_param != 0 and (100 - dummy_param) / neutrality_param < 5:
                            continue

                        print('{}, {}, {}, {}, {}, {}'.format(dimension, dummy_param, neutrality_param, epistasis_param, ruggedness_param, 800 if dimension == 16 else dimension * 10))
                        nbSol = 800 if dimension == 16 else dimension * 10

                        out = subprocess.Popen(['../ifmo/cmake-build-debug/w_model',
                                                '-n={}'.format(nbSol),
                                                '-V={}'.format(dimension),
                                                '-D={}'.format(dummy_param / dimension if dummy_param / dimension < 1 else 0.999),
                                                '-N={}'.format(neutrality_param),
                                                '-E={}'.format(epistasis_param),
                                                '-R={}'.format(ruggedness_param)],
                                               stdout=subprocess.PIPE,
                                               stderr=subprocess.STDOUT)
                        stdout, stderr = out.communicate()

                        result = str(stdout)
                        values = []

                        # Adaptive walk
                        values.append(float(result.split('Mean adaptive walk length: ')[1].split('\\n')[0]))

                        # Autocorrelation
                        values.append(float(result.split('1 rho=')[1].split(',')[0]))
                        values.append(float(result.split('2 rho=')[1].split(',')[0]))
                        values.append(float(result.split('3 rho=')[1].split(',')[0]))

                        values.append(float(result.split('1 rho=')[1].split('phi=')[1].split('\\n')[0]))
                        values.append(float(result.split('2 rho=')[1].split('phi=')[1].split('\\n')[0]))
                        values.append(float(result.split('3 rho=')[1].split('phi=')[1].split('\\n')[0]))

                        # Density of states
                        values.append(float(result.split('Min fitness value: ')[1].split('\\n')[0]))
                        values.append(float(result.split('Max fitness value: ')[1].split('\\n')[0]))
                        values.append(float(result.split('Mean fitness value: ')[1].split('\\n')[0]))
                        values.append(float(result.split('Std fitness value: ')[1].split('\\n')[0]))

                        # FDC
                        values.append(float(result.split('Fitness-distance correlation: ')[1].split('\\n')[0]))

                        # Fitness cloud
                        values.append(float(result.split('Fitness-neighbor correlation: ')[1].split('\\n')[0]))

                        # Neutral degree
                        values.append(float(result.split('Min neutral degree: ')[1].split('\\n')[0]))
                        values.append(float(result.split('Max neutral degree: ')[1].split('\\n')[0]))
                        values.append(float(result.split('Mean neutral degree: ')[1].split('\\n')[0]))
                        values.append(float(result.split('Std neutral degree: ')[1].split('\\n')[0]))

                        # Neutral walk
                        values.append(float(result.split('Min neutral walk: ')[1].split('\\n')[0]))
                        values.append(float(result.split('Max neutral walk: ')[1].split('\\n')[0]))
                        values.append(float(result.split('Mean neutral walk: ')[1].split('\\n')[0]))
                        values.append(float(result.split('Std neutral walk: ')[1].split('\\n')[0]))

                        if len(headers) != len(values):
                            print('Not enough values')
                            exit(1)

                        spamwriter.writerow(values)

