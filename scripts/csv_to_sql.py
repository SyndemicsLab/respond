import sqlite3
import pandas as pd


def insert_inputs(csv_dir_path, conn: sqlite3.Connection):
    input_files = ["background_mortality.csv",
                   "entering_cohort.csv", "healthcare_utilization_cost.csv",
                   "init_cohort.csv",
                   "utility.csv", "SMR.csv"]
    cur = conn.cursor()

    dfs = []
    for f in input_files:
        dfs.append(pd.read_csv(csv_dir_path + "/" + f, index_col=False))

    background_mortality_query = """
    INSERT INTO background_mortality (age_group, race, sex, year, death_probability, input_set_id)
        SELECT a.id, r.id, s.id, y.id, ?, 1
        FROM age_group AS a, race AS r, sex AS s, years AS y
        WHERE a.age_group = ? AND r.race = ? AND s.sex = ? AND y.year = ?;"""

    # dfs[0]["year"] = 2015
    # dfs[0] = dfs[0][['death_prob', 'agegrp', 'race', 'sex', 'year']]
    # data = list(dfs[0].itertuples(index=False, name=None))
    # cur.executemany(background_mortality_query, data)

    utility_query = """
    INSERT INTO utility (intervention, age_group, race, sex, background, setting, input_set_id)
        SELECT i.id, a.id, r.id, s.id, ?, ?, 1
        FROM intervention AS i, age_group AS a, race AS r, sex AS s
        WHERE i.intervention = ? AND a.age_group = ? AND r.race = ? AND s.sex = ?;"""

    # dfs[4] = dfs[4][['background', 'setting', 'block',
    #                  'agegrp', 'race', 'sex']]
    # data = list(dfs[4].itertuples(index=False, name=None))
    # cur.executemany(utility_query, data)

    cost_query = """
    INSERT INTO cost (intervention, behavior, age_group, race, sex, year, healthcare, input_set_id)
        SELECT i.id, b.id, a.id, r.id, s.id, y.id, ?, 1
        FROM intervention AS i, behavior AS b, age_group AS a, race AS r, sex AS s, years AS y
        WHERE i.intervention = ? AND b.behavior = ? AND a.age_group = ? AND r.race = ? AND s.sex = ? AND y.year = ?;"""

    # dfs[3]["year"] = 2015
    # dfs[3] = dfs[3][['healthcare_system', 'block',
    #                  'oud', 'agegrp', 'race', 'sex', 'year']]
    # data = list(dfs[3].itertuples(index=False, name=None))
    # cur.executemany(cost_query, data)

    entering_cohort_query = """
    INSERT INTO entering_cohort (intervention, behavior, age_group, race, sex, year, weekly_cohort_size_change, input_set_id)
        SELECT 0, 0, a.id, r.id, s.id, y.id, ?, 1
        FROM age_group AS a, race AS r, sex AS s, years AS y
        WHERE a.age_group = ? AND r.race = ? AND s.sex = ? AND y.year = ?;"""

    # dfs[2]["year"] = 2015
    # dfs[2] = dfs[2][['cohort_size_change_1_52', 'agegrp', 'race', 'sex', 'year']]
    # data = list(dfs[2].itertuples(index=False, name=None))
    # cur.executemany(entering_cohort_query, data)

    init_cohort_query = """
    INSERT INTO init_cohort (intervention, behavior, age_group, race, sex, year, counts, input_set_id)
        SELECT i.id, b.id, a.id, r.id, s.id, y.id, ?, 1
        FROM intervention AS i, behavior AS b, age_group AS a, race AS r, sex AS s, years AS y
        WHERE i.intervention = ? AND b.behavior = ? AND a.age_group = ? AND r.race = ? AND s.sex = ? AND y.year = ?;"""

    # dfs[4]["year"] = 2015
    # dfs[4] = dfs[4][['counts', 'block', 'oud', 'agegrp', 'race', 'sex', 'year']]
    # data = list(dfs[4].itertuples(index=False, name=None))
    # cur.executemany(init_cohort_query, data)

    smr_query = """
    INSERT INTO smr (intervention, behavior, age_group, race, sex, year, smr, input_set_id)
        SELECT i.id, b.id, a.id, r.id, s.id, y.id, ?, 1
        FROM intervention AS i, behavior AS b, age_group AS a, race AS r, sex AS s, years AS y
        WHERE i.intervention = ? AND b.behavior = ? AND a.age_group = ? AND r.race = ? AND s.sex = ? AND y.year = ?;"""

    # dfs[8]["year"] = 2015
    # dfs[8] = dfs[8][['SMR', 'block', 'oud', 'agegrp', 'race', 'sex', 'year']]
    # data = list(dfs[8].itertuples(index=False, name=None))
    # cur.executemany(smr_query, data)


def insert_params(csv_dir_path, conn):
    param_files = ["all_types_overdose.csv", "block_init_effectv2.csv",
                   "block_trans.csv", "fatal_overdose.csv", "oud_trans.csv"]

    cur = conn.cursor()

    dfs = []
    for f in param_files:
        dfs.append(pd.read_csv(csv_dir_path + "/" + f, index_col=False))

    overdose_query = """
    INSERT INTO overdoses (intervention, behavior, age_group, race, sex, year, 
                            percent_pop_overdoses, percent_overdoses_fatal, parameter_set_id)
        SELECT i.id, b.id, a.id, r.id, s.id, y.id, ?, ?, 1
        FROM intervention AS i, behavior AS b, age_group AS a, race AS r, sex AS s, years AS y
        WHERE i.intervention = ? AND b.behavior = ? AND a.age_group = ? AND r.race = ? AND s.sex = ? AND y.year = ?;
    """

    # df_od = dfs[0].merge(dfs[3], how='inner')
    # df_od['year'] = 2015
    # df_od = df_od[['overdose_1_52', 'percent_overdoses_fatal_1_52',
    #                'intervention', 'oud', 'agegrp', 'race', 'sex', 'year']]
    # data = list(df_od.itertuples(index=False, name=None))
    # cur.executemany(overdose_query, data)

    block_init_effect_query = """
    INSERT INTO behavior_transition_after_intervention_change (initial_behavior, 
                            new_intervention, year, retention_probability, parameter_set_id)
        SELECT b.id, i.id, y.id, ?, 1
        FROM behavior AS b, intervention AS i, years AS y
        WHERE b.behavior = ? AND i.intervention = ? AND y.year = ?;
    """

    # dfs[1]['year'] = 2015
    # dfs[1] = dfs[1][['retention_rate', 'initial_oud_state', 'to_intervention', 'year']]
    # data = list(dfs[1].itertuples(index=False, name=None))
    # cur.executemany(block_init_effect_query, data)

    block_trans_query = """
    INSERT INTO intervention_transitions (start_intervention, behavior, age_group, race, sex, year, 
                            end_intervention, transition_probability, parameter_set_id)
        SELECT si.id, b.id, a.id, r.id, s.id, y.id, ni.id, ?, 1
        FROM intervention AS si, behavior AS b, age_group AS a, race AS r, sex AS s, years AS y, intervention AS ni
        WHERE si.intervention = ? AND b.behavior = ? AND a.age_group = ? AND r.race = ? AND s.sex = ? AND y.year = ? AND ni.intervention = ?;
    """

    # df = dfs[2]
    # df['year'] = 2015
    # df = df.loc[:, ~df.columns.str.contains('^Unnamed')]
    # df = df.melt(id_vars=['agegrp', 'race', 'sex', 'oud', 'initial_intervention',
    #              'year'], var_name='end_intervention', value_name='transition_probability')
    # df['end_intervention'] = df['end_intervention'].str[:-5]

    # df = df[['transition_probability', 'initial_intervention',
    #          'oud', 'agegrp', 'race', 'sex', 'year', 'end_intervention']]
    # data = list(df.itertuples(index=False, name=None))
    # cur.executemany(block_trans_query, data)

    oud_trans_query = """
    INSERT INTO behavior_transitions (intervention, start_behavior, age_group, race, sex, year, 
                            end_behavior, transition_probability, parameter_set_id)
        SELECT i.id, sb.id, a.id, r.id, s.id, y.id, eb.id, ?, 1
        FROM intervention AS i, behavior AS sb, age_group AS a, race AS r, sex AS s, years AS y, behavior AS eb
        WHERE i.intervention = ? AND sb.behavior = ? AND a.age_group = ? AND r.race = ? AND s.sex = ? AND y.year = ? AND eb.behavior = ?;
    """

    # df = dfs[4]
    # df['year'] = 2015
    # df = df.loc[:, ~df.columns.str.contains('^Unnamed')]
    # df = df.melt(id_vars=['agegrp', 'race', 'sex', 'initial_oud', 'intervention',
    #              'year'], var_name='end_behavior', value_name='transition_probability')

    # df = df[['transition_probability', 'intervention', 'initial_oud',
    #          'agegrp', 'race', 'sex', 'year', 'end_behavior']]
    # data = list(df.itertuples(index=False, name=None))
    # cur.executemany(oud_trans_query, data)


if __name__ == "__main__":
    db_file_path = "/home/matt/Repos/TestData/RESPOND/new_inputs/data.db"
    conn = sqlite3.connect(db_file_path)
    insert_params(
        '/home/matt/Repos/TestData/RESPOND/new_inputs/input1002', conn)
    conn.commit()
