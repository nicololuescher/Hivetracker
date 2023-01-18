const Sequelize = require('sequelize');
const config = require('./hivetracker-config.json');
const sequelize = new Sequelize(process.env.DB_SCHEMA || 'postgres',
                                process.env.DB_USER || config.postgresConfig.postgresUser,
                                process.env.DB_PASSWORD || config.postgresConfig.postgresPassword,
                                {
                                    host: process.env.DB_HOST || config.postgresConfig.postgresHost,
                                    port: process.env.DB_PORT || config.postgresConfig.postgresPort,
                                    dialect: 'postgres',
                                    dialectOptions: {
                                        ssl: process.env.DB_SSL == "true"
                                    }
                                });

const Hives = sequelize.define('Hives', {
    hiveName: {
        type: Sequelize.STRING,
        allowNull: false
    },
    hiveLocation: {
        type: Sequelize.STRING,
        allowNull: false
    },
    hiveDescription: {
        type: Sequelize.STRING,
        allowNull: false
    },
    hiveWeightOffset: {
        type: Sequelize.FLOAT,
        allowNull: true
    },
    hiveWeightCallibrationFactor: {
        type: Sequelize.FLOAT,
        allowNull: true
    }
});

const Temperature = sequelize.define('Temperature', {
    temperature: {
        type: Sequelize.FLOAT,
        allowNull: false
    },
    hiveId: {
        type: Sequelize.INTEGER,
        allowNull: false
    }
});

const Weight = sequelize.define('Weight', {
    weight: {
        type: Sequelize.FLOAT,
        allowNull: false
    },
    hiveId: {
        type: Sequelize.INTEGER,
        allowNull: false
    }
});

module.exports = {
    sequelize: sequelize,
    Hives: Hives,
    Temperature: Temperature,
    Weight: Weight
};